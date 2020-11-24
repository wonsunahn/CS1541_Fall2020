# CS/COE 1541 Midterm 2 Exam Study Guide - Fall 2020

The midterm is on Sunday (11/29) 11:59 AM - Tuesday (12/1) 11:59 PM.  It's
going to be open book but individual work.  I will not take any questions
during the midterm about the material that we learned, just like any exam.  The
exam will be released on GradeScope on the date of the exam.  The mechanics of
taking the exam on GradeScope is going to be the same as for Midterm 1.  Since
it is open book,  you can refer to all past material as well as the textbook.
You may even use the simulator to simulate your answers!

Midterm 2 will cover everything starting from Exceptions all the way to GPUs.
The exam is not cumulative.  I recommend you review the slides and use the
textbook for reference if you don't fully understand a concept.  Going over the
TopHat questions and your homeworks will be quite helpful too.  

The exam will consist of these types of questions:
  * Multiple choice questions
  * Fill in the blank questions
  * Short answer questions (explain a concept, discuss pros/cons, etc.)
  * Calculation questions that apply concepts to the given scenario
  * 'Drawing' questions where you are asked to describe the cache state at each cycle 
  * GPU CUDA code tracing questions where you will be asked about resulting output and performance
  * GPU CUDA coding questions where you will be asked to complete the missing parts of a kernel

Here are the key topics to study in preparation for the test.

## Exceptions

* Be able to define what an exception is
* Be able to define what a precise exception is
  * And why it is important
* Be able to explain why precise exceptions in out-of-order superscalars are difficult
* Be able to explain how internal state (and out-of-order execution) is hidden from visible state
  * Registers: through the use of physical registers
  * Memory: through the use of a store queue
* Be able to explain what roles the instruction queue and retirement register file have in precise exceptions
  * Instruction queue: guarantees that instructions commit their state in program order
  * Retirement register file: holds the current values of architectural registers at all times

## Memory Hierarchy

* Be able to compare the pros / cons of each memory technology
  * SRAM
  * DRAM
  * HDD
  * Flash
* Be able to explain why memory is constructed as a hierarchy mentioning:
  * Inverse relationship between speed vs capacity
  * Typical program data access patterns
* Be able to identify the memory technology used in each level of a typical memory hierarchy
* Be able to explain pros / cons of caching
  * Pro: on average, short memory access time
  * Con: variability in memory access time and end performance
* Be able to explain the rationale behind caching: locality
  * Temporal locality: be able to define precisely
  * Spatial locality: be able to define precisely
* Be able to define 3 different types of cache misses
  * Cold misses (compulsory misses)
  * Capacity misses
  * Conflict misses (explained later in the Cache Design chapter)
* Be able to explain why load latency has a more direct impact to performance compared to store latency
  * Loads read in data that is needed for computation to proceed -> on the critical path
  * Stores write data to memory that is the result of computation -> off the critical path
* Be able to explain role a write buffer plays in keeping a pending store off the critical path
* Be able to read a cache miss rate plot and analyze it and tell which levels are getting accessed
* Be able to explain the relationship between program working set and cache miss rates at a certain cache level
  * Also be able to explain why as working set increases cache miss rates often exhibit a step function
* Be able to compare different data structure memory access patterns and tell the temporal or spatial locality leveraged by a cache
  * Linked list. with different node sizes
  * Array, with different element sizes

## Cache Design

* Be able to define what cpu bound and memory bound mean
  * Be able to categorize computer system optimizations between those that benefit cpu bound and those that benefit memory bound programs
* Be able to calculate AMAT (average memory access time) given a memory hierarchy and hit times and miss rates at each level.
  * Be able to justify choosing one cache design over another
* Be able to explain impact of cache capacity on AMAT
  * On hit times
  * On miss rates
  * Be able to explain why higher capacity is used for the last level cache
* Be able to explain impact of cache block size on AMAT
  * On miss rates
  * On miss penalties
* Be able to explain impact of cache associativity on AMAT
  * On miss rates
  * On hit times
  * Be able to explain why fully-associative caches have lower miss rates compared to direct mapped
* Be able to calculate which cache block a memory address would map to, and the cache tag, given
  * Cache block size
  * Cache capacity
  * Cache associativity
* Be able to "simulate" cache hits and misses given a sequence of memory accesses
* Be able to explain why LRU (Least Recently Used) policy takes better advantage of temporal locality
* Be able to explain impact of LRU policy on AMAT
  * On miss rates (either smaller or larger)
* Be able to relate different cache design parameters to the two different types of locality, or three different types of cache misses:
  * E.g. cache capacity -> temporal locality, capacity misses
  * E.g. cache block size -> spatial locality, capacity misses, cold misses
  * E.g. cache associativity -> temporal locality, conflict misses

## Cache Design 2

* Be able to explain the cache consistency problem and how write-back and write-through caches solve it respectively
* Be able to "simulate" L1 and L2 caches for a stream of memory accesses for both write-back and write-through policies
* Be able to explain the drawbacks of a write-through policy
  * From the perspective of memory latency -> need a very big write buffer to store pending writes and what problems that cause
  * From the perspective of bandwidth -> be able to explain why write buffers don't help in this respect
* Be able to explain why write-allocate policy is used for write-back caches
  * On the flip side, be able to explain why no-write-allocate policy is used for write-through caches
  * Be able to explain why on write allocate, you first need to read the cache block from lower memory
* Be able to explain impact of write policy on AMAT
  * On hit times
  * On miss penalties
  * And the role write buffers play to mitigate each issue
* Be able to explain impact of unified vs. split caches have on AMAT
  * On miss rates (related to the flexible use of space)
  * On hit times (due to structural hazards)
* Be able to explain the rationale behind shared caches, instead of private caches
  * Related to the use of cache space in a multiprocessor machine
* Be able to explain why shared caches can suffer more from structural hazards
  * Be able to explain what "banking" is and how it mitigates this problem
* Be able to explain why shared caches can suffer from longer (and non-uniform) hit times
* Be able to define what prefetching is
  * And be able to explain why it can work on programs that exhibit no temporal or spatial locality
* Be able to explain the two different modes of prefetching
  * Software driven
  * Hardware driven
* Be able to identify access patterns that a hardware prefetcher can easily take advantage of
* Be able to explain why the timing of the prefetching is as important as the target of the prefetching

## Virtual Memory and Caching

* Be able to apply concepts in caching with concepts in paging
  * Why virtual memory systems have 4 KB pages
  * Why pages are fully associative and a relatively complex replacement policy like LRU is used
  * Why write-back policy is used for pages
* Be able to explain when page table lookups happen and with what frequency
* Be able to explain the rationale behind a hierarchical page table
  * Be able to explain the downside of a hierarchical page table in relation to the number of accesses
* Be able to define what a TLB (Translation Lookaside Buffer) is: a cache
* Be able to explain two ways of handling a TLB miss
  * Through OS exceptions
  * Through a hardware page table walker
* Be able to explain at least 2 ways caching speed up memory access, besides the fact that L1,L2,L3 caches cache program data
  * DRAM acts as a cache for HDD
  * TLB acts as a cache for PTEs
  * Page tables are stored in memory, so PTEs can also take advantage of L1,L2,L3 caches

## Multiprocessors and Caching

* Be able to explain why the act of caching data can cause problems with shared data in a multiprocessor system
  * Be able to explain why the problem only occurs for private caches and not shared caches
* Be able to define what cache coherence means
* Be able to identify the three states in the MSI protocol: modified, shared, and invalid
  * Be able to explain what each state entails
* Given a stream of memory accesses, be able to "simulate" cache block state in each processor cache
* Be able to explain how TLBs maintain coherence between TLBs when a PTE is modified

## SIMD and GPUs

* Be able to define what SIMD means and what kind of worloads can benefit from it
  * Be able to explain what vector instructions mean
  * Be able to explain what vector registers mean
* Be able to identify 3 reasons why vector instructions are faster than conventional instructions
  * No need for loop control and indexing overhead
  * No need to fetch and decode the same instrucion over and over again
  * Can process multiple data items in parallel with multiple PEs (processing elements)
* Be able to explain what gather/scatter means in terms of vector instructions and why it can be bad for performance
* Be able to explain what it means to say that a GPU is really a SIMD machine
  * Be able to explain that SMs are SIMD processors
  * Be able to explain that in GPUs, a "warp" is a vector instruction
* Be able to explain the thread execution model of GPUs
  * How thread blocks are scheduled on to SMs
  * How each thread block is divided into warps (vector instructions) that are scheduled on SMs
  * How each thread gets mapped on to an SP
* Be able to explain the GPU programming model 
  * Be able to explain why calls like cudaMalloc and cudaMemCpy are needed
  * Be able to explain what a grid is, what a thread block is, what a thread is
  * Be able to explain what a kernel is and its relationship with a GPU thread
* Be able to explain how each thread in a kernel is able to perform different computation on different data items (and not do the same thing over and over again)
  * Be able to use threadIdx, blockIdx, and blockDim to index into different data items 
* Be able to explain why branches and control divergence within a warp can cause low utilization of GPU resources
  * Be able to explain the role predicates play in implementing branches in GPUs
* Be able to explain what TLP (Thread Level Parallelism) is and its role in GPU performance
  * Be able to explain why certain kernel layouts perform better than others, even when using the same number of threads
* Be able to identify the two points of bandwidth bottleneck that is critical to GPU performance
  * PCIe bus that is used for cudaMemCpys back and forth between the CPU memory (system memory) and GPU memory (global memory).
  * Memory bus between GPU and GPU DRAM (global memory).
* Given a GPU performance plot and GPU performance counter plots, be able to explain why a certain workload (e.g. matrix-vector multiplication) does not scale to many threads.
* Be able to define what arithmetic intensity means
  * Be able to explain what implication low or high arithmetic intensity has on GPU performance
