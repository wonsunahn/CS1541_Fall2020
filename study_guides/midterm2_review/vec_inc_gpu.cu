#include <algorithm>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <cuda_runtime.h>

int N = 1024;		// length of vector A
float* d_A = NULL;	// Pointer to vector A in device memory

double time_memcpy = 0;
double time_compute = 0;	// Timers

/////////////////////////////////////////////////////////////////////////
// Matrix-matrix Multiplication on CPU: Policy 1
/////////////////////////////////////////////////////////////////////////
void inc_cpu(float* A, int n, int inc_count) {
	for (int i=0; i<n; i++) {
		for (int j=0; j<inc_count; j++) {
			A[i] += 1;
		}
	}	
}

/////////////////////////////////////////////////////////////////////////
// Matrix-matrix Multiplication on GPU: Policy 2
/////////////////////////////////////////////////////////////////////////
__global__ void inc_gpu(float* A, int n, int inc_count)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	for (int j=0; j<inc_count; j++) {
		A[i] += 1;
	}
}

/////////////////////////////////////////////////////////////////////////
// Macro to check that CUDA API calls are successful
/////////////////////////////////////////////////////////////////////////
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

/////////////////////////////////////////////////////////////////////////
// Allocate device memory and copy from host to device memory
/////////////////////////////////////////////////////////////////////////
void copy_host_to_device(float* A, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	cudaMalloc((void**) &d_A, sizeof(float) * n * n);
	assert(d_A);
	cudaMemcpy(d_A, A, sizeof(float) * n, cudaMemcpyHostToDevice);

	gettimeofday (&tv ,   &tz);
	time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	time_memcpy += time_end - time_start;
}

/////////////////////////////////////////////////////////////////////////
// Copy result from device to host memory and free device memory
/////////////////////////////////////////////////////////////////////////
void copy_device_to_host(float* A, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	// TODO:
	// 1. Copy result array d_C to C using cudaMemcpy.
	// 2. Free memory allocated for d_A, d_B, d_C using cudaFree.

	cudaMemcpy(A, d_A, sizeof(float) * n, cudaMemcpyDeviceToHost);
	cudaFree(d_A);

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

	if(argc != 5) {printf("Usage: mat_mul_gpu <array size> <block size> <policy (1|2|3)> <(debug|nodebug)>\n") ; exit(2) ;}
	int inc_count = atoi(argv[1]) ;		// number of times to increment each element
	int block_size = atoi(argv[2]) ;	// size of thread block
	int policy = atoi(argv[3]) ;		// 1 or 2
	bool debug = (!strcmp(argv[4], "debug")) ? true : false;

	// allocate host memory for vector A
	float* h_A = (float*)malloc(sizeof(float) * N);

	// initialize host memory with zeros
	for (int i = 0; i < N; ++i){
		h_A[i] = 0;
	}

	//call the relevant policy 
	if(policy==1){ // CPU baseline
		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		inc_cpu(h_A, N, inc_count);

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;
	}
	else if(policy==2){ // GPU
		copy_host_to_device(h_A, N);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

                dim3 dimBlock(block_size);
                dim3 dimGrid(N / dimBlock.x);
		inc_gpu <<<dimGrid, dimBlock>>> (d_A, N, inc_count);

		gpuErrchk( cudaPeekAtLastError() );
		gpuErrchk( cudaDeviceSynchronize() );

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;

		copy_device_to_host(h_A, N);
	}

	if (debug) {
		// print selected results (for checking correctness)
		for (int i=0; i<N; i+=N/10) { printf(" %10.6f",h_A[i]); }
		printf("\n");
	}
	if(!debug) {
		printf("Memcpy Time: %f seconds\n", time_memcpy);
		printf("Compute Time: %f seconds\n", time_compute);
	}

	// clean up memory allocated  
	free(h_A);

	cudaDeviceReset();
}
