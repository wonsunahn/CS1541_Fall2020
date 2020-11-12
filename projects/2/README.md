- [CS/COE 1541 - Introduction to Computer Architecture](#cscoe-1541---introduction-to-computer-architecture)
- [Introduction](#introduction)
  * [Description](#description)
  * [Processor Design](#processor-design)
- [Building and Running](#building-and-running)
  * [Environment Setup](#environment-setup)
  * [Directory Structure and Makefile Script](#directory-structure-and-makefile-script)
  * [Program Output](#program-output)
    + [Solution Binary](#solution-binary)
    + [Current Binary](#current-binary)
  * [Configuration Files](#configuration-files)
  * [Trace Files](#trace-files)
- [Your Tasks](#your-tasks)
  * [Task 1: Implement the Cache Block Array](#task-1-implement-the-cache-block-array)
  * [Task 2: Implement the Write-back and Write-through Caches](#task-2-implement-the-write-back-and-write-through-caches)
  * [Source Code](#source-code)
  * [Submission](#submission)
- [Resources](#resources)
  
# CS/COE 1541 - Introduction to Computer Architecture
Fall Semester 2020 - Project 2

* Source Code DUE: Dec 4 (Friday), 2020 11:59 PM

# Introduction

## Description

The goal of this project is to create a software simulator for an in-order
processor with a configurable memory hierarchy.  I've already explained why
building simulators is important in Project 1, so I will not belabor that
point. :)

## Processor Design

The processor that you will simulate is configured with a memory hierarchy that
looks as follows:

<img alt="Memory hierarchy" src=Project2_diagram.png width=700>

We will only test a 1-wide processor because we are focusing on memory stalls
this time.  You will also ignore all pipeline hazards for the same reason.

The processor has split data and instruction L1 caches and a unified L2 cache
below it.  Below that sits the DRAM memory.  In the test configurations, the L2
cache is always configured as a write-back (WB) cache to reduce bandwidth
consumption to memory.  The L1 cache is configured as either a write-through
(WT) cache or a write-back (WB) cache.  Write-back and write-through caches
work in exactly the same way as explained during the lecture.  Please review
the lecture if you don't remember.

Each cache has a dedicated write-buffer for write-back memory requests.  The
processor pipeline also has a write-buffer to maintain pending stores with long
delays.  We are going to assume for simplicity that all the write-buffers are
infinitely sized.  That means that the processor will never suffer stalls due
to write-backs of dirty blocks or store instructions.

# Building and Running

## Environment Setup

The project is setup to run with the g++ compiler (GNU C++ compiler) and a Make
build system.  This system is already in place on the departmental Linux
machines (linux.cs.pitt.edu).  If you have a similar setup on your local
computer, please feel free to use your machine for development.  Otherwise, you
need log in to linux.cs.pitt.edu which may involve some setup.  Here are the
steps you need to take:

1. If you haven't already, install Pulse VPN Desktop Client.  Instructions on how to download and use:
https://www.technology.pitt.edu/services/pittnet-vpn-pulse-secure
Then, set up the VPN client and connect to Pitt VPN  as follows:
https://www.technology.pitt.edu/help-desk/how-to-documents/pittnet-vpn-pulse-secure-connect-pulse-secure-client

2. Most OSes (Windows, MacOS, Linux) comes with built-in SSH clients accessible using this simple command on your commandline shell:
   ```
   ssh USERNAME@linux.cs.pitt.edu
   ```
   If you want a more fancy SSH client, you can download Putty, a free open source terminal:
   https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
   Connect to "linux.cs.pitt.edu" by typing that in the "Host Name" box.  Make sure that port is 22 and SSH is selected in the radio button options.

3. Once connected, the host will ask for your Pitt SSO credentials.  Enter your ID and password.

4. Once logged in, check your quota by typing "fs lq":
   ```
   $ fs lq
   Volume Name                    Quota       Used %Used   Partition
   u.wahn                       2547712    1543899   61%         85%
   ```
   This shows your quota in KBs.  The above shows 2.5 GBs.  This project will require at least 50 MBs of disk space.  If you don't have that much remaining please contact my.pitt.edu to increase it.  By default, students are given a paltry 5 MBs of space so it is likely that you don't have the space.  I have been told that you can be given up to 2 GBs of space if you ask for it.  The fastest way to contact my.pitt.edu is through the **live chat** on the website.  It should just take a minute or two.

The project files are within the directory /afs/cs.pitt.edu/courses/1541/project2 once you are logged in to linux.cs.pitt.edu.  Identical files are also on this GitHub folder.  Copy the project files to a working directory of your choice and cd into that directory.

## Directory Structure and Makefile Script

Here is an overview of the directory structure:

```
# Source code you will be **modifying**.
CacheCore.cpp / CacheCore.h : A cache block array organized into rows and columns.
Cache.h / Cache.cpp : Contains classes for a write-back cache and a write-through cache.

# Source code newly added as part of Project 2.
CacheLine.h : A cache line (a.k.a. a cache block) with tag, valid bit, dirty bit, and age.
Counter.h : A counter, pure and simple.
DRAM.h : DRAM memory, which mostly acts like a cache that always hits.
MemObj.cpp / MemObj.h : Parent class for all memory objects (caches and DRAM).
MemRequest.cpp / MemRequest.h : Memory request that gets passed around memory objects.
log2i.cpp / log2i.h : Contains the log2i function, a log2 for integers.

# Directory newly added as part of Project 2.
doc/ : Directory where Doxygen documentation of the source code is kept.

# Source code inherited from Project 1 with small modifications.
config.c / config.h : Functions used to parse and read in the processor configuration file.
CPU.c / CPU.h : Implements the five stages of the processor pipeline, modified to consider memory stalls.
five_stage.c : Main function. Parses commandline arguments and invokes the five stages at every clock cycle.
trace.c / trace.h : Functions to read and write the trace file.
trace_generator.c : Utility program to generate a trace file of your own.
trace_reader.c : Utility program to read and print out the contents of a trace file in human readable format.

# Files inherited from Project 1 with some modifications.
five_stage_solution : **Reference solution binary** for the project.
Makefile : The build script for the Make tool.
confs/ : Directory where processor configuration files are.
diffs/ : Directory with diffs between outputs/ and outputs_solution/ are stored.
outputs/ : Directory where outputs after running five_stage are stored.
outputs_solution/ : Directory where outputs produced by five_stage_solution are stored.
plot_confs/ : Directory where processor configurations for the plot generation are.
plots/ : Directory where outputs after running five_stage are stored for plot generation.
plots_solution/ : Directory where outputs after running five_stage_solution are stored for plot generation.
traces/ : Directory where instruction trace files used to test the simulator are stored.
```

In order to build the project and run the simulations, you only need to do 'make' to invoke the 'Makefile' script:

```
$ make
g++ -c -g -Wall -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ five_stage.c
g++ -c -g -Wall -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ config.c
g++ -c -g -Wall -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ CPU.c
g++ -c -g -Wall -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ trace.c
...
```

If successful, it will produce the binaries: five_stage, trace_generator, and
trace_reader as well as results of the simulation using all combinations of 2
configuration files and 9 trace files.  A configuration file represents a
processor design and a trace file contain instructions from a micro-benchmark,
so you will be in effect simulating 2 processor designs on a benchmark suite.
You can generate your own traces using the trace_generator and put it inside
the traces/ directory or create a new configuration inside the confs/
directory, and they will be incorporated into the results automatically by the
Makefile script.  The results are stored in the outputs/ directory and also
side-by-side diffs with the outputs_solution/ directory are generated and
stored in the diffs/ directory.  When you debug the program, you will find
these side-by-side diffs useful.

If you only wish to build your C++ files and not run the simulations, just do
'make build' to invoked the 'build' target in the 'Makefile' script:

```
$ make build
```

If you wish to remove all files generated from your five_stage implementation (object files and experiment output), invoke the 'clean' target:

```
$ make clean
```

If you wish to remove all generated files (including ones generated from five_stage_solution), invoke the 'distclean' target:

```
$ make distclean
```

You can also run your simulator on more sizable benchmarks.  I have 4 short and
2 long trace files: (sample1.tr, sample2.tr, sample3.tr, sample4.tr) and
(sample_large1.tr, sample_large2.tr). These files are accessible at
/afs/cs.pitt.edu/courses/1541/long_traces and
/afs/cs.pitt.edu/courses/1541/short_traces. But these are not incorporated into
the Makefile default target because they take significantly longer to run.
When you do run these on five_stage, I recommend you do not have the -v
(verbose) or -d (debug) flags on or the simulations will take too long and the
output may overflow your disk space.

## Program Output

As before five_stage.c reads in a trace file (a binary file containing a
sequence of executed instructions) and simulates the processor described above.
But since the implementation is as of now incomplete, the L1 and L2 caches will
always miss and you will always go to DRAM memory.  Your goal is to complete
the cache implementation so that the accesses sometimes hit in the caches.

### Solution Binary

Let's start by looking at five_stage_solution (the solution binary) to see how
the output *should* look like.

Try doing the following:

```
$ ./five_stage_solution -t traces/two_loads.tr -c confs/l1-wb.conf -d
```

Or, alternatively just open the 'outputs_solution/two_stores.l1-wb.out' file after doing 'make'.

And you should see the following output at the beginning:

```
Memory system setup successful.
======================================================================

Printing all memory objects ...

[DL1Cache]
device type = cache
write policy = WB
hit time = 2
capacity = 8192
block size = 64
associativity = 1
lower level = L2Cache

[IL1Cache]
device type = cache
write policy = WB
hit time = 2
capacity = 8192
block size = 64
associativity = 1
lower level = L2Cache

[L2Cache]
device type = cache
write policy = WB
hit time = 10
capacity = 16384
block size = 64
associativity = 4
lower level = Memory

[Memory]
device type = dram
hit time = 100

======================================================================
...
```

This is just saying that the configuration file has been successfully parsed
and the memory hierarchy objects have been created and initialized correctly
with the configuration parameters.  You can see that all caches are configured
to be 'WB' or write-back.

Next, you see the following output:
```
[IF CYCLE: 1] STORE: (Seq:        1)(Addr: 39168)(PC: 0)
IL1Cache->access(MemRead, addr: 0, latency: 2)
L2Cache->access(MemRead, addr: 0, latency: 12)
Memory->access(MemRead, addr: 0, latency: 112)
CYCLE: 1 -> 112
======================================================================
Printing all cache contents ...
[DL1Cache]
[IL1Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
======================================================================
```

This shows your first memory access.  It is saying that the instruction fetch
(PC: 0) for the store instruction generated a read memory request (addr: 0) to
the instruction L1 cache.  Since initially the cache is empty, it will go down
all the way to memory to fetch that cache block.  You can see that at each
level of the memory hierarchy, latency increases equal to the configured hit
time parameter for that memory object.  When the memory request returns, the
current CYCLE is incremented by 'latency - 1' (1 -> 112).  One is subtracted
because one cycle is the default latency for the MEM stage in the pipeline and
anything beyond that is what causes a stall.

Next, the cache contents of all the caches are dumped to output.  Only valid
blocks are dumped.  The block string representation is interpreted as follows:

```
(0, 0) tag=0:valid=1:dirty=0:age=0
```

This means that this cache block is in row 0, column 0 of the cache block
array.  Row is the set index and column is the index of the cache block within
the set.  Please use the cache visualizer under resources/cache_demo/ of the
repository to visualize the rows and columns.  The rest is the various metadata
for that cache block.

Next, you see the following output:
```
[IF CYCLE: 113] STORE: (Seq:        2)(Addr: 39200)(PC: 4)
IL1Cache->access(MemRead, addr: 4, latency: 2)
CYCLE: 113 -> 114
======================================================================
Printing all cache contents ...
[DL1Cache]
[IL1Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
======================================================================
```

You see an instruction fetch for the next PC: 4.  Due to spatial locality, this
read to the instruction L1 cache hits.  You can see that this time, the memory
request just accesses the instruction L1 cache and immediately returns.  The
cache contents remain the same since no new blocks are allocated.

Next, you see the following output:
```
[MEM CYCLE: 116] STORE: (Seq:        1)(Addr: 39168)(PC: 0)
DL1Cache->access(MemWrite, addr: 39168, latency: 2)
L2Cache->access(MemRead, addr: 39168, latency: 12)
Memory->access(MemRead, addr: 39168, latency: 112)
CYCLE: 116 -> 116
======================================================================
Printing all cache contents ...
[DL1Cache]
(100, 0) tag=4:valid=1:dirty=1:age=0
[IL1Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
(25, 0) tag=9:valid=1:dirty=0:age=0
======================================================================
```

You see the first STORE instruction performing the write access at the MEM
stage with addr: 39168.  It misses in the data L1 cache, which means that a new
block must be allocated in the L1 and the data for the block read in from lower
memory.  So the MemWrite request is now mutated into a MemRead request to
access the L2 cache.  It misses there again and you have to go all the way
memory.  Now note the line CYCLE: 116 -> 116.  We don't add stall cycles for
the store instruction because we assumed we have an infinitely sized write
buffer.  So conceptually, all of this activity is happening off the critical
path as the processor is executing subsequent instructions.

Looking at the cache contents, we see the new cache block read into the L2 cache:

```
(25, 0) tag=9:valid=1:dirty=0:age=0
```

It's clean because it hasn't been modified.  We also see the new cache block in the IL1 cache:

```
(100, 0) tag=4:valid=1:dirty=1:age=0
```

This block is dirty because it was written to with the store data.

Next, you see the following output:
```
[MEM CYCLE: 117] STORE: (Seq:        2)(Addr: 39200)(PC: 4)
DL1Cache->access(MemWrite, addr: 39200, latency: 2)
CYCLE: 117 -> 117
======================================================================
Printing all cache contents ...
[DL1Cache]
(100, 0) tag=4:valid=1:dirty=1:age=0
[IL1Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
[L2Cache]
(0, 0) tag=0:valid=1:dirty=0:age=0
(25, 0) tag=9:valid=1:dirty=0:age=0
======================================================================
```

This is the MEM stage for the second store to addr: 39200.  The previous
address 39168 and 39200 are on the same cache block so the data L1 cache hit.

Next, you see a summary of statistics for each memory object:
```
======================================================================

Printing all memory stats ...

DL1Cache:readHits=0:readMisses=0:writeHits=1:writeMisses=1:writeBacks=0
IL1Cache:readHits=1:readMisses=1:writeHits=0:writeMisses=0:writeBacks=0
L2Cache:readHits=0:readMisses=2:writeHits=0:writeMisses=0:writeBacks=0
Memory:readHits=2:writeHits=0

======================================================================
```

There are a few invariants here:

* L2Cache hits = DL1Cache misses + IL1Cache misses
* Memory hits = L2Cache misses

Make sure they hold in your code too.

And then some final statistics:
```
+ Memory stall cycles : 112
+ Number of cycles : 118
+ IPC (Instructions Per Cycle) : 0.0169
```

### Current Binary

As I said, the implementation is incomplete, so the L1 and L2 caches will
always miss and you will always go to DRAM memory.  Let's look at five_stage
(the current binary) to see how the output looks like *now*.

Try doing the following:

```
$ ./five_stage -t traces/two_loads.tr -c confs/l1-wb.conf -d
```

Or, alternatively just open the 'outputs/two_stores.l1-wb.out' file after doing 'make'.

And you should see the following output after the preamble in the beginning:

```
======================================================================
[IF CYCLE: 1] LOAD: (Seq:        1)(Addr: 39168)(PC: 0)
IL1Cache->access(MemRead, addr: 0, latency: 2)
L2Cache->access(MemRead, addr: 0, latency: 12)
Memory->access(MemRead, addr: 0, latency: 112)
CYCLE: 1 -> 112
======================================================================
Printing all cache contents ...
[DL1Cache]
[IL1Cache]
[L2Cache]
======================================================================
[IF CYCLE: 113] LOAD: (Seq:        2)(Addr: 39200)(PC: 4)
IL1Cache->access(MemRead, addr: 4, latency: 2)
L2Cache->access(MemRead, addr: 4, latency: 12)
Memory->access(MemRead, addr: 4, latency: 112)
CYCLE: 113 -> 224
======================================================================
Printing all cache contents ...
[DL1Cache]
[IL1Cache]
[L2Cache]
======================================================================
```

You can see that on the instruction fetch for the first store, the read misses
at all levels of the cache.  But afterwards, no blocks are added to any of the
caches.  This causes the second instruction fetch to also miss, even though it
is to the same cache block.  Your job is to make this work.

## Configuration Files

You can find 2 configuration files under the confs/ directory.  Each will
configure a different processor when passed as the -c option to five_stage.
The files are: l1-wb.conf and l1-wt.conf.

* l1-wb.conf.conf : Configuration where L1 caches are write-back (WB) caches.
* l1-wt.conf.conf : Configuration where L1 caches are write-through (WT) caches.

Other than the difference in the L1 write policy, all other parameters are equal.

Here is how the l1-wb.conf file looks like:

```
# Processor pipeline
[pipeline]
width         = 1
instSource    = IL1Cache
dataSource    = DL1Cache

# Instruction L1 cache
[IL1Cache]
deviceType    = cache
size          = 8192            # 8 * 1024
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 2
lowerLevel    = L2Cache

# Data L1 cache
[DL1Cache]
deviceType    = cache
size          = 8192            # 8 * 1024
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 2
lowerLevel    = L2Cache

# L2 cache
[L2Cache]
deviceType    = cache
size          = 16384           # 16 * 1024
assoc         = 4
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 10
lowerLevel    = Memory

# DRAM memory
[Memory]
deviceType    = dram
size          = 64
assoc         = 1
bsize         = 64
writePolicy   = WB
replPolicy    = LRU
hitDelay      = 100
lowerLevel    = null
```

Here is what each of those items mean:

[pipeline]
* width = 1 : It is a 1-wide processor.
* instSource = IL1Cache : The pipeline uses IL1Cache as its data source.
* dataSource = DL1Cache : The pipeline uses DL1cache as its data source.

[IL1Cache]
* deviceType = cache : The device is a cache (not dram)
* size = 8192 : Capacity is 8 KB
* assoc = 1 : Associativity is 1-way (a.k.a. direct-mapped)
* bsize = 64 : Cache block size is 64 bytes
* writePolicy = WB : Write policy is write-back (not write-through)
* replPolicy = LRU : Replacement policy is LRU (this is the only option)
* hitDelay = 2 : Delay required to access to cache is 2 cycles
* lowerLevel = L2Cache : The memory object below this level is L2Cache

The instSource, dataSource, and lowerLevel parameters name a memory object by
the section name that defines that object.  In this way, the memory hierarchy
can be configured with an arbitrary number of levels.

## Trace Files

You can find 8 trace files under the traces/ directory.  I've listed them in the orer of difficulty.

* one_load.tr : One load instruction.
* one_store.tr : One store instruction.
* two_loads.tr : Two load instructions.  The second load hits due to spatial locality.
* two_stores.tr : Two store instructions.  The second store hits due to spatial locality.
* many_loads.tr : Many load instructions.  The many loads all land on the same set and cause cache block replacements at multiple levels of the cache.  LRU replacement policy is tested as well.
* many_stores.tr : Many store instructions.  The many stores all land on the same set and cause cache block replacements at multiple levels of the cache.  LRU replacement policy is tested as well.  The additional difficulty here is that since the blocks are dirty, write-backs will need to occur for write-back caches.
* many_loads_then_stores.tr : All the loads in many_loads.tr followed by all the stores in many_stores.tr. 
* many_stores_then_loads.tr : The opposite of many_loads_then_stores.tr.
* sample.tr : A moderately long trace of instructions (681 instructions).

# Your Tasks

All the places where you have to complete code is clearly marked with '// TODO'
comments.  The only files that you would have to modify are CacheCore.cpp /
CacheCore.h and Cache.cpp / Cache.h.  While completing the methods with '//
TODO' comments, feel free to add more member variables and member functions to
the above files as required.

## Task 1: Implement the Cache Block Array

Complete the implementation of CacheCore.cpp / CacheCore.h.  The 'content'
cache block array has been already allocated for you.  It is up to you to
interpret that array to the row and column organization shown in the cache
visualizer for a set-associative cache.  Start by implementing the index2Row
and index2Column functions as specified according to the configured
associativity.  Then go on to implement the accessLine and allocateLine
functions that searches for a block and allocates block in the cache
respectively.

## Task 2: Implement the Write-back and Write-through Caches

Complete the implementation of Cache.h / Cache.cpp.  As of now, all accesses
miss in the cache and no cache block allocation is done.  Complete the read,
write, and writeBack functions according to what we learned in the lecture.

## Source Code

I had to write some of the code in C++ this time because there was no way to
cleanly implement memory objects with just C.  While you may have never learned
C++, don't get intimidated, it's just C with classes thrown in.  The syntax for
C++ classes is almost identical to Java classes with small variations.  

Since you are using C++ for the first time, I made sure to rigorously document
all the variables and functions so everything is clear.  I also used
[Doxygen](https://www.doxygen.nl/index.html) to auto-generate HTML
documentation from the source code comments.  The HTML files are under
doc/html/ and you can open it with any web browser.  Start from 'index.html'.
Click on the 'Classes' tab then choose 'MemObj'.  That's a good place to start
because it shows you the class inheritance hierarchy starting from MemObj which
you can navigate by clicking on any of the children classes.  Pay special
attention to 'CacheCore', 'WBCache', and 'WTCache' documentation since those
are the classes that you will be modifying.  Doxygen only works with classes so
C functions are not listed.

For those of you who are not familiar with C++, here are a few pointers to
differences with Java:

1. Inheritance

   The syntax for inheritance is:

   ```
   class Cache: public MemObj
   ```

   This means that Cache inherits from the MemObj class.  The 'public' specifier
means that public members in MemObj remain public in Cache.

1. Overriding Methods and Abstract Methods

   In C++, you may often see the 'virtual' keyword before a function:

   ```
   virtual void read(MemRequest *mreq) = 0;
   ```

   Unlike Java, if you want to override a method in the child class, you have to
declare it as 'virtual' in the parent class.  The above method is declared as
part of the Cache class and it is declared as virtual because it is overriden
in the children classes WBCache and WTCache.

   In addition, the ' = 0;' notation says that, this method is an *abstract
method*.  An abstract method is a method with no implementation.  So it's much
like an interface in Java in spirit.  Any class with an abstract method that is
not overriden is called an *abstract class* and objects cannot be instantiated
for an abstract class.  Much like how interfaces cannot be instantiated for
Java.  So you cannot create a Cache object in this case.

1. Creating and Freeing Objects

   Creating objects in C++ is almost identical to Java: you use the 'new' keyword.
The key difference is in freeing of objects.  Java does automatic garbage
collection.  With C++, the programmer has to manually free the object using the
'delete' keyword:

   ```
   mreq = new MemRequest(dinst.inst.Addr, MemRead);
   ...
   delete mreq;
   ```

   As you can see in the above code in CPU.c, you have to delete every object you
create using 'new' or else you will have a memory leak.

As with C programs, you can use
[valgrind](https://valgrind.org/docs/manual/QuickStart.html) to detect most
memory bugs in C++, including memory leaks.

## Submission

Each pairwise group will submit the exercise *once* to GradeScope, by *one member* of the group.  The submitting member will press the "View or edit group" link at the top-right corner of the assignment page after submission to add his/her partner.  That way, both of you will get a grade.  This applies to both the Project 1 Source Code and Project 1 Retrospective submissions explained below.

You will do two submissions for this deliverable.

1. **(90 points)** Project 1 Source Code (Due Dec. 4 11:59 PM)
   
   To be released on GradeScope.  As for Project 1, the grading will be based on your diff results.

1. **(20 points)** Project 1 Retrospective (Due Dec. 4 11:59 PM)

   To be released on GradeScope.  

# Resources

* Valgrind Memory Checker Tutorial: https://valgrind.org/docs/manual/QuickStart.html
* Windows SSH Terminal Client: [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
* File Transfer Client: [FileZilla](https://filezilla-project.org/download.php?type=client)
* Linux command line tutorial: [The Linux Command Line](http://linuxcommand.org/lc3_learning_the_shell.php)
* GitHub tutorial: [Using Git](https://github.com/wonsunahn/CS1541_Fall2020/blob/master/lectures/Using_Git.pdf)
* GitHub GUI Client: [GitHub Desktop](https://desktop.github.com/)
