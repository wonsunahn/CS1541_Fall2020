#include <algorithm>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <cuda_runtime.h>

float* d_A = NULL;	// Pointer to matrix A in device memory
float* d_x = NULL;	// Pointer to vector x in device memory
float* d_y = NULL;	// Pointer to result vector y in device memory

double time_memcpy = 0;
double time_compute = 0;	// Timers

/////////////////////////////////////////////////////////////////////////
// Matrix-vector Multiplication on CPU: Policy 1 
/////////////////////////////////////////////////////////////////////////
void mv_cpu(float* y, const float* A, const float* x, int n) {
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			y[i] += A[i*n + j] * x[j];
		}
	}	
}

/////////////////////////////////////////////////////////////////////////
// Matrix-vector Multiplication on GPU: Policy 2 
/////////////////////////////////////////////////////////////////////////
__global__ void mv_cuda_simple(float* y, float* A, float* x, int n)
{
	int row = blockIdx.x * blockDim.x + threadIdx.x;
	if (row < n) {
		float temp = 0;		
		for (int k = 0; k < n; k++) {
			temp += A[row*n + k] * x[k];
		}
		y[row] = temp;
	}
}

/////////////////////////////////////////////////////////////////////////
// Matrix-vector Multiplication on GPU using shared memory: Policy 3 
/////////////////////////////////////////////////////////////////////////
__global__ void mv_cuda_shared(float* y, float* A, float* x, int n)
{
	extern __shared__ float s_x[];
	int row = blockIdx.x * blockDim.x + threadIdx.x;
	if (row < n) {
		float temp = 0;		
		for (int j = 0; j < n; j += blockDim.x) {
			if (j + threadIdx.x < n) {
				s_x[threadIdx.x] = x[j + threadIdx.x];
			}
			__syncthreads();

			for (int k = j; k < min(n, j + blockDim.x); k++) {
				temp += A[row*n + k] * s_x[k - j];
			}
		}
		y[row] = temp;
	}
}

/////////////////////////////////////////////////////////////////////////
// Allocate device memory and copy from host to device memory
/////////////////////////////////////////////////////////////////////////
void copy_host_to_device(float* A, float* x, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	cudaMalloc((void**) &d_A, sizeof(float) * n * n);
	cudaMalloc((void**) &d_x, sizeof(float) * n);
	cudaMalloc((void**) &d_y, sizeof(float) * n);
	assert(d_A);
	assert(d_x);
	assert(d_y);
	cudaMemcpy(d_A, A, sizeof(float) * n * n, cudaMemcpyHostToDevice);
	cudaMemcpy(d_x, x, sizeof(float) * n, cudaMemcpyHostToDevice);

	gettimeofday (&tv ,   &tz);
	time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	time_memcpy += time_end - time_start;
}

/////////////////////////////////////////////////////////////////////////
// Copy result from device to host memory and free device memory
/////////////////////////////////////////////////////////////////////////
void copy_device_to_host(float* y, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	cudaMemcpy(y, d_y, sizeof(float) * n, cudaMemcpyDeviceToHost);
	cudaFree(d_A);
	cudaFree(d_x);
	cudaFree(d_y);

	gettimeofday (&tv ,   &tz);
	time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	time_memcpy += time_end - time_start;
}

/////////////////////////////////////////////////////////////////////////
// Program main
/////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	if(argc != 5) {printf("Usage: mat_vec_gpu <array size> <block size> <policy (1|2|3)> <(debug|nodebug)>\n") ; exit(2) ;}
	int N = atoi(argv[1]) ;				// array size
	int block_size = atoi(argv[2]) ;	// size of thread block
	int policy = atoi(argv[3]) ;		// 1, 2 or 3
	bool debug = (!strcmp(argv[4], "debug")) ? true : false;

	if(debug) {
		printf("\n Array size = %d , block size = %d and policy = %d\n ", N, block_size, policy);
	}

	// set seed for rand()
	srand(2020);

	// allocate host memory for matrix A and vector x
	int matrix_size = N * N;
	float* h_A = (float*)malloc(sizeof(float) * matrix_size);
	float* h_x = (float*)malloc(sizeof(float) * N);

	// randomly initialize host memory
	for (int i = 0; i < matrix_size; ++i){
		h_A[i] = rand() / (float)RAND_MAX;
	}
	for (int i = 0; i < N; i++){
		h_x[i] = rand() / (float)RAND_MAX;
	}

	// allocate host memory for the result
	float* h_y = (float*)malloc(sizeof(float) * N);
	for (int i = 0; i < N; i++) {
		h_y[i] = 0.0;
	}

	//call the relevant policy 
	if(policy==1){ //CPU baseline
		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		mv_cpu(h_y, h_A, h_x, N);

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;
	}
	else if(policy==2){ //CUDA MV multiplication
		// call mv_cuda_simple <<< >>> ( )

		copy_host_to_device(h_A, h_x, N);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		int nblocks = (N + block_size - 1) / block_size;
		mv_cuda_simple <<<nblocks, block_size>>> (d_y, d_A, d_x, N);
		cudaDeviceSynchronize();

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;

		copy_device_to_host(h_y, N);
	}
	else if(policy==3){ //CUDA MV multiplication with shared memory
		// call mv_cuda_shared<<<grid, block_size >>>(d_y, d_A, d_x, N);

		copy_host_to_device(h_A, h_x, N);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		int nblocks = (N + block_size - 1) / block_size;
		mv_cuda_shared <<<nblocks, block_size, sizeof(float) * block_size>>> (d_y, d_A, d_x, N);
		cudaDeviceSynchronize();

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;

		copy_device_to_host(h_y, N);
	}

	if (debug) {
		// print selected results (for checking correctness) and print time elapsed
		for (int i=0; i<N; i+=N/10) { printf(" %10.6f",h_y[i]); }
		printf("\n");
	}
	printf("Memcpy Time: %f seconds\n", time_memcpy);
	printf("Compute Time: %f seconds\n", time_compute);

	// clean up memory allocated  
	free(h_A);
	free(h_x);
	free(h_y);

	cudaDeviceReset();
}
