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
float* d_B = NULL;	// Pointer to matrix B in device memory
float* d_C = NULL;	// Pointer to result matrix C in device memory

double time_memcpy = 0;
double time_compute = 0;	// Timers

/////////////////////////////////////////////////////////////////////////
// Matrix-matrix Multiplication on CPU: Policy 1
/////////////////////////////////////////////////////////////////////////
void mm_cpu(float* C, const float* A, const float* B, int n) {
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
                        C[i*n + j] = 0;
			for (int k=0; k<n; k++) {
				C[i*n + j] += A[i*n + k] * B[k*n + j];
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// Matrix-matrix Multiplication on GPU: Policy 2
/////////////////////////////////////////////////////////////////////////
__global__ void mm_gpu(float* C, float* A, float* B, int n)
{
	// TODO:
	// Implement the naive GPU matrix multiplication shown on the lecture slides.
	// Remove the printf below when you don't need it.  It's just for demonstration.
	printf("Grid(%d, %d) Block (%d, %d)\n", blockIdx.x, blockIdx.y, threadIdx.x, threadIdx.y);
}

/////////////////////////////////////////////////////////////////////////
// Matrix-matrix Multiplication on GPU using shared memory: Policy 3
/////////////////////////////////////////////////////////////////////////
__global__ void mm_gpu_shared(float* C, float* A, float* B, int n)
{
	int block_size = blockDim.x;
	// Shared memory used to store a tile in arrays A and B respectively.
	// We needed to allocate dynamic shared memory because the block size is unknown ahead of time.
        // This looks strange but if you read the below NVIDIA blog entry, it will make sense:
	// https://developer.nvidia.com/blog/using-shared-memory-cuda-cc/
        // This is equivalent to the below static shared memory declarations (but it won't work because of block_size).
	// __shared__ float As[block_size * block_size];
	// __shared__ float Bs[block_size * block_size];
	extern __shared__ float As[];
	float *Bs = &As[block_size * block_size];

	// TODO:
	// Implement GPU matrix multiplication using tiling and the above allocated shared memory.
	
	// For reference, you can look at the following 2 codes:
	// 1. A tiled vector-matrix multiplication implementation is available in /resources/gpu_experiments/mat_vec_gpu.cu of this repository.
	// 2. A tiled matrix multiplication for a special case where the "w" (row size of A and column size of B) is small is shown in:
	// https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#shared-memory-in-matrix-multiplication-c-ab
	// Note that a tile in the NVIDIA code is 2-dimensional when As and Bs above declares a 1-dimensional array.
	// (Because you cannot declare dynamic 2-dimensional array in C/C++.)
	// That means you will have to use that 1-d array as a 2-d array using the old "row*width + column" translation.
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
void copy_host_to_device(float* A, float* B, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	// TODO:
	// 1. Allocate GPU memory for d_A, d_B, d_C using cudaMalloc.
	// 2. Copy input arrays A, B to d_A, d_B using cudaMemcpy.
	// 3. Call cudaMemset on array d_C to initialize all elements to 0.

	gettimeofday (&tv ,   &tz);
	time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	time_memcpy += time_end - time_start;
}

/////////////////////////////////////////////////////////////////////////
// Copy result from device to host memory and free device memory
/////////////////////////////////////////////////////////////////////////
void copy_device_to_host(float* C, int n)
{
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv ,   &tz);
	time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

	// TODO:
	// 1. Copy result array d_C to C using cudaMemcpy.
	// 2. Free memory allocated for d_A, d_B, d_C using cudaFree.

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
	int N = atoi(argv[1]) ;				// array size
	int block_size = atoi(argv[2]) ;	// size of thread block
	int policy = atoi(argv[3]) ;		// 1, 2 or 3
	bool debug = (!strcmp(argv[4], "debug")) ? true : false;

	//printf("\n Array size = %d , block size = %d and policy = %d\n ", N, block_size, policy);

	// set seed for rand()
	srand(2020);

	// allocate host memory for matrix A and vector x
	int matrix_size = N * N;
	float* h_A = (float*)malloc(sizeof(float) * matrix_size);
	float* h_B = (float*)malloc(sizeof(float) * matrix_size);
	float* h_C = (float*)malloc(sizeof(float) * matrix_size);

	// randomly initialize host memory
	for (int i = 0; i < matrix_size; ++i){
		h_A[i] = rand() / (float)RAND_MAX;
	}
	for (int i = 0; i < matrix_size; i++){
		h_B[i] = rand() / (float)RAND_MAX;
	}
	for (int i = 0; i < matrix_size; i++){
		h_C[i] = 0.0;
	}

	//call the relevant policy 
	if(policy==1){ //CPU baseline
		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		mm_cpu(h_C, h_A, h_B, N);

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;
	}
	else if(policy==2){ //CUDA MV multiplication
		copy_host_to_device(h_A, h_B, N);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		// TODO: Call the kernel
		// Call mm_gpu <<< >>> ( ) with the appropriate grid and thread block layouts.

		gpuErrchk( cudaPeekAtLastError() );
		gpuErrchk( cudaDeviceSynchronize() );

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;

		copy_device_to_host(h_C, N);
	}
	else if(policy==3){ //CUDA MV multiplication with shared memory
		copy_host_to_device(h_A, h_B, N);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

                dim3 dimBlock(block_size, block_size);
                dim3 dimGrid(N / dimBlock.x, N / dimBlock.y);
		// Additional 3rd parameter "sizeof(float) * block_size * block_size * 2" is used to specify how much shared memory to allocate for a thread block.
		// It's needed because we want to dynamically allocate shared memory according to the given thread block size.
		mm_gpu_shared <<<dimGrid, dimBlock, sizeof(float) * block_size * block_size * 2>>> (d_C, d_A, d_B, N);

		gpuErrchk( cudaPeekAtLastError() );
		gpuErrchk( cudaDeviceSynchronize() );

		gettimeofday (&tv ,   &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
		time_compute = time_end - time_start;

		copy_device_to_host(h_C, N);
	}

	if (debug) {
		// print selected results (for checking correctness)
		for (int i=0; i<matrix_size; i+=matrix_size/10) { printf(" %10.6f",h_C[i]); }
		printf("\n");
	}
	if(!debug) {
		printf("Memcpy Time: %f seconds\n", time_memcpy);
		printf("Compute Time: %f seconds\n", time_compute);
	}

	// clean up memory allocated  
	free(h_A);
	free(h_B);
	free(h_C);

	cudaDeviceReset();
}
