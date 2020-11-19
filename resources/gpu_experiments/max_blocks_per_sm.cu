#include <stdio.h>
#include <sys/time.h>

double cpuSecond() {
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return (double) tp.tv_sec + (double)tp.tv_usec*1e-6;
}

__device__ void sleep(float t, clock_t clock_rate) {    
    clock_t t0 = clock64();
    clock_t t1 = t0;
    while ((t1 - t0)/(clock_rate*1000.0f) < t)
        t1 = clock64();
}

__global__ void mykernel(clock_t clock_rate) {
    sleep(1.0, clock_rate);    
}

int main(int argc, char* argv[]) {
    cudaDeviceProp  prop;
    cudaGetDeviceProperties(&prop, 0); 
    int mp = prop.multiProcessorCount;
    clock_t clock_rate = prop.clockRate;

    int max_num_blocks = atoi(argv[1]);
    
    for (int num_blocks = 1; num_blocks <= max_num_blocks; num_blocks++) {
	dim3 block(1);
	dim3 grid(num_blocks);  /* N blocks */

	double start = cpuSecond();
	mykernel<<<grid,block>>>(clock_rate);
	cudaDeviceSynchronize();
	double etime = cpuSecond() - start;

	printf("blocks      %10d\n",num_blocks);
	printf("SMs         %10d\n",mp);
	printf("blocks/SM   %10.2f\n",num_blocks/((double)mp));
	printf("time        %10.2f\n",etime);
        printf("\n");
    }

    cudaDeviceReset();
}
