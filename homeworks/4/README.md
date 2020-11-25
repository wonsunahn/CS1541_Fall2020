# CS/COE 1541 - Introduction to Computer Architecture
Fall Semester 2020 - Homework 4

* DUE: Dec 4 (Friday), 2020 11:59 PM

# Introduction

## Description

The goal of this homework is to practice GPU programming using the CUDA
language and experience first hand improving the performance of GEMM (General
Matrix Multiplication) which is a staple in many important applications such as
machine learning.

# Building and Running

## Environment Setup

The homework is setup to run with the nvcc compiler (NVIDIA CUDA compiler) and
a Make build system.  This system is already in place on a few machines with
GPUs that I rented out from the department.  The hostnames of these machines are:

* pc6110-e1.cs.pitt.edu
* pc6110-e2.cs.pitt.edu
* pc6110-e3.cs.pitt.edu
* pc6110-e4.cs.pitt.edu
* pc6110-e5.cs.pitt.edu

You need to log in to one of these machines to do the work.

1. If you haven't already, install Pulse VPN Desktop Client.  Instructions on how to download and use:

   https://www.technology.pitt.edu/services/pittnet-vpn-pulse-secure

   Then, set up the VPN client and connect to Pitt VPN  as follows:

   https://www.technology.pitt.edu/help-desk/how-to-documents/pittnet-vpn-pulse-secure-connect-pulse-secure-client

2. Most OSes (Windows, MacOS, Linux) comes with built-in SSH clients accessible using this simple command on your commandline shell:
   ```
   ssh USERNAME@pc6110-e1.cs.pitt.edu
   ```
   If you want a more fancy SSH client, you can download Putty, a free open source terminal:
   https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
   Connect to "linux.cs.pitt.edu" by typing that in the "Host Name" box.  Make sure that port is 22 and SSH is selected in the radio button options.

3. Once connected, the host will ask for your Pitt SSO credentials.  Enter your ID and password.

The homework files are within the directory
/afs/cs.pitt.edu/courses/1541/homework4 once you are logged in to a machine.
Identical files are also on this GitHub folder.  Copy the homework files to a
working directory of your choice and cd into that directory.

I recommend using your AFS home directory as your working directory.  Because the default home directory in the GPU machines is in the local hard disk so that if you go and try to use another GPU machine, that home directory will not be visible to you.  If you want to know your AFS home directory, just log in to linux.cs.pitt.edu and try doing pwd:

```
(1064) kernighan $ pwd
/afs/pitt.edu/home/w/a/wahn
```

## Directory Structure and Makefile Script

Here is an overview of the directory structure:

```
# Source code you will be **modifying**.
mat_mul_gpu.cu: Matrix multiplication program that can perform the multiplication on either:
Policy 1 - the CPU, Policy 2 - the GPU, and Policy 3 - the GPU using shared memory.

# Other scripts and directories
Makefile : The build script for the Make tool.
generate_plot.py : Script to extract performance numbers from the outputs and store in tabular format.
generate_mat_mul_plot.plt : GNUPlot script to convert the data table to a PDF plot.
diffs/ : Directory where diffs between CPU and GPU outputs are stored.
outputs/ : Directory where outputs after running the program are stored.
```

In order to build the project and run the experiments, you only need to do 'make' to invoke the 'Makefile' script:

```
$ make
/opt/cuda-7.0/bin/nvcc  devicequery.cu -o devicequery
/opt/cuda-7.0/bin/nvcc  mat_mul_gpu.cu -o mat_mul_gpu
...
```

If successful, it will produce the binaries: devicequery and mat_mul_gpu.
Devicequery is just a short program that outputs device characteristics of your
GPU (not needed for the homework really, but just for information purposes).
Mat_mul_gpu is your matrix multiplication program.

Make will also generate results of experiments using all combinations of the
three policies and various thread block sizes.  The results are stored in the
outputs/ directory and also side-by-side diffs between Policy 1 vs. Policy 2
and Policy 1 vs.  Policy 3 are generated and stored in the diffs/ directory.
When you debug the program, you will find these side-by-side diffs useful.  The
output is selected elements from the result matrix and if you did your GPU
programming correctly, then it should match the output from the CPU (Policy 1)
--- minus some floating point imprecision that has already been taken into
account in the diff.py Python script.

If you only wish to build your CUDA files and not run the experiments, just do
'make build' to invoked the 'build' target in the 'Makefile' script:

```
$ make build
```

If you wish to remove all files generated (object files and experiment output), invoke the 'clean' target:

```
$ make clean
```

# Your Tasks

All the places where you have to complete code is clearly marked with '// TODO'
comments.  

## Task 1: Complete the scaffolding code to invoke the GPU

Complete the copy_host_to_device and copy_device_to_host functions required to
copy the data back and forth between the CPU and GPU.  Review the examples and
the APIs in the slides.  Then, insert the call to launch the mm_gpu kernel
using a two dimensional grid and a two dimensional thread block.  Again, follow
the example on the slides.  Once you are done, you should see the printf in
mm_gpu printing to the screen:

```
[wahn@pc6110-e1 4]$ ./mat_mul_gpu 1024 32 2 nodebug
Grid(1, 31) Block (0, 11)
Grid(1, 31) Block (1, 11)
Grid(1, 31) Block (16, 11)
Grid(1, 31) Block (2, 11)
Grid(1, 31) Block (3, 11)
Grid(1, 31) Block (17, 11)
Grid(1, 31) Block (4, 11)
...
```

You can see the output generated by each thread and the coordinate of each
thread in the kernel.  One strange thing that I found with this particular GPU
is that it does not print out all the blocks in the grid.  But when I limit the
output to a particular block using an if statement, it does print out.  I
believe it's due to having too much output at once?  I am going to chalk this
up to a quirk in this particular version.  But otherwise, printfs should work
fine.

This means you can use the printfs to debug your CUDA code, just like your C
code.  One caveat: you cannot use printfs within the \_\_global\_\_ device function
to print out host memory (your system memory).  Vice-versa, you can't use
printfs within your host code (your main function) to print out device memory
(GPU memory).  Which makes complete sense, right?  They can't see each other's
memories.

## Task 2: Implement the naive GPU matrix multiply (mm_gpu)

Complete the implentation by looking at the lecture slides.  It should be
pretty straighforward, really.  Don't forget to remove the above printf.  If
you've done your job, the diffs for Policy 2 should start to pass.

For example, if you see the following:

```
[wahn@pc6110-e1 4]$ cat diffs/mat_mul_gpu.2.32.diff
Diff success!
```

Then you've done a good job.  The diff files named mat_mul_gpu.2.*.diff are the result of diffing Policy 2 (mm_gpu) against Policy 1 (mm_cpu).
Again, don't forget to remove the spurious printf or the diff will always give you a failure.

## Task 3: Implement the optimized GPU matrix multiply (mm_gpu_shared)

This version uses shared memory and loop tiling to speed up the matrix
multiply.  I have already declared the shared memory for you with the
\_\_shared\_\_ modifier.  I did this part for you because allocating dynamic shared
memory is a bit hairy.

This will be a bit more challenging.  But if you go to the linked NVIDIA page in the code:
https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#shared-memory-in-matrix-multiplication-c-ab

It explains to you what needs to be done.  Note how after filling in the shared
memory using multiple threads, there is a \_\_syncthreads() call.  That call is
important to ensure that the shared memory is completely filled before using
it.  It is a form of thread synchronization so that we don't have data races
where threads try to read a shared memory that is not yet filled.  

If you want to learn more about loop tiling, please read the wikipedia page:
https://en.wikipedia.org/wiki/Loop_nest_optimization#Example:_matrix_multiplication

If you've done your job, the diffs for Policy 3 should also pass.

For example, if you see the following:

```
[wahn@pc6110-e1 4]$ cat diffs/mat_mul_gpu.3.32.diff
Diff success!
```

Then you've done a good job.  The diff files named mat_mul_gpu.3.*.diff are the result of diffing Policy 3 (mm_gpu_shared) against Policy 1 (mm_cpu).

## Task 4: Check your performance!

Open the generated MatMulTime.pdf plot and observe that you see big speedups like in this plot:

<img alt="Matrix Multiplication Plot" src=MatMulTime_Solution.png width=700>

## Submission

You will do one submission for this homework to the GradeScope "Homework 4" link.  Since the GradeScope machines do not have GPUs it will not be autograded, but it will be graded manually according to the rubric explained on the GradeScope assignment.

# Resources

* identical 3 policies but for the matrix vector multiplication I showed on the alides:
  [/resources/gpu_experiments/mat_vec_gpu.cu](/resources/gpu_experiments/mat_vec_gpu.cu)
* NVIDIA guide on how to use shared memory for matrix multiplication:
  https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#shared-memory-in-matrix-multiplication-c-ab
