# CS/COE 1541 Midterm 1 Exam Study Guide - Fall 2020

The midterm is on Friday (10/9) 10:00 AM - Sunday (10/11) 10:00 AM.  It's going
to be open book but individual work.  I will not take any questions during the
midterm about the material that we learned, just like any exam.  The exam will
be released on GradeScope on the date of the exam.  The mechanics of taking the
exam on GradeScope is going to be the same as what you did for Homework 1 and 2.
Since it is open book,  you can refer to all past material as well as the
textbook.  You may even use the simulator to simulate your answers!

The midterm will cover the introduction and pipeling, all the way up to and
including superscalars and exceptions.  I recommend you review the slides and
use the textbook for reference if you don't fully understand a concept.  Going
over the TopHat questions and your homeworks will be quite helpful too.  

The exam will consist of these types of questions:
  * Multiple choice questions
  * Fill in the blank questions
  * Short answer questions (explain a concept, discuss pros/cons, etc.)
  * Calculation questions that apply concepts to the given scenario
  * 'Drawing' questions where you are asked to describe the pipeline state at each cycle 

Here are the key topics to study in preparation for the test.

## Introduction

* Be able to define what Computer Architecture is
  * Be able to define ISA (Instruction Set Architecture)
  * Be able to define processor organization
* Be able to describe application pull and examples of it
* Be able to explain the reasoning behind the statement "an architecture is a compromise"

## Technology Advances

* Be able to define what Moore's Law is
  * And what it isn't!
* Be able to calculate execution time given the three components of performance
* Be able to differentiate the impact various factors have on MOSFET transistor speed
  * Channel length (Gate length)
  * Capacitance
  * Supply voltage
  * Threshold voltage
* Be able to explain why processor performance started to stagnate around 2003
  * Be able to explain it relation to Thermal Deisgn Power (TDP)
* Be able to explain the impact of Moore's Law on each component of CPU power
* Be able to explain Dennard Scaling
  * Which component of CPU power could be reduced (Hint: supply voltage)
  * How supply voltage could be reduced without reducing transistor speed (Hint: by reducing threshold voltage)
* Be able to explain why Dennard Scaling stopped (Hint: something to do with leakage power)

## Technology Constraints

* Be able to explain the causes and effects of the Power Wall constraint
* Be able to explain techniques to work around the Power Wall constraint and whether they affect dynamic or static power
  * Clock gating
  * Power gating
  * DVFS
  * Increasing threshold voltage
* Be able to calculate the impact of various techniques on dynamic and static power (refer to HW1)
* Be able to explain the causes and effects of the Memory Wall constraint
* Be able to explain techniques to work around the Memory Wall constraint 
  * Memory caches
  * Stacked memory
* Be able to explain the causes and effects of the Variability constraint
* Be able to explain techniques to work around the Variability constraint 
  * Product binning
  * Multicores
  * Limited pipelining

## Processor Review

* Be able to define clock cycle, propagation delay, and critical path
* Be able to explain what determines clock cycle
* Be able to explain how control signals are generated in a processor
* Be able to explain the values of control signals for a given instruction (e.g. add)
* Be able to explain in what situations multicycle processor is faster than an equivalent single cycle processor.

## Processor Pipelining

* Be able to compare impact on the three components of performance for single-cycle / multi-cycle / pipelined
* Be able to calculate the impact given a scenario (refer to HW1)
* Be able to explain the causes and effects of three types of hazards
  * Structural hazards
  * Data hazards
  * Control hazards
* Be able to explain how the processor deals with each type of hazard
  * With compiler scheduling - insert nops
  * With dynamic hazard detection unit (HDU) - insert stalls or flush instructions
* Be able to compare the pros / cons of compiler nops vs. HDU
* Be able to draw a pipeline execution with no hazards
* Be able to draw a pipeline execution with various hazards (refer to HW2)

## Optimizing Pipeline Hazards

* Be able to explain two optimizations for structural hazards:
  * Functional units: duplication
  * Memory structures: adding more read / write ports
* Be able to explain two optimizations for data hazards:
  * Data forwarding (and also the limits of forwarding for use-after-load)
  * Reordering instructions
* Be able to show a reordering of instructions by the compiler to resolve stalls, given code with stalls
* Be able to explain limitations of compiler reordering
  * Compiler must make assumptions about the pipeline
  * Compiler cannot accurately predict memory access latency at compile time
  * Compiler has difficulty in accurately analyzing data dependencies
* Be able to draw a pipeline execution with hazards optimization techniques (refer to HW2)
* Be able to explain four optimizations for structural hazards:
  * Delay slots (and also its limitations)
  * A branch condition comparator in the ID stage (and its limitations)
  * Branch prediction (next chapter)
  * Predication (next chapter)

## Branch Prediction and Predication

* Be able to calculate the performance impact of branch prediction
* Be able to compare static and dynamic branch prediction.  What is the prediction based upon?
* Be able to explain the use and composition of the Branch History Table (BHT)
  * Used to predict branch direction
  * Indexed by hash of PC giving taken or not-taken result
* Be able to calculate the prediction rate of 1-bit and 2-bit BHTs, given a piece of code
* Be able to explain the use and composition of the Branch Target Buffer (BTB)
  * Used to predict the branch target
  * Be able to explain what happens when you don't have a BTB and why it's needed
* Be able to identify the pipeline stage where BHT and BTB are placed: IF
  * Be able to explain how BHT and BTB can give a result even before decode
  * Be able to explain why the additional steps in the IF stage does not increase delay of IF stage
* Be able to explain the use and composition of the Correlating Predictor
  * Used to predict branch direction for branches with complex behaviors
  * Indexed by hash of PC + Local Branch History + Global Branch History
  * One Global BHR, multiple local BHRs for each branch
* Be able to explain the use and composition of the Return Stack Buffer
  * Used to predict the branch target of function return instructions
  * Composed of a stack of return addresses that you push and pop
* Be able to define what a Predicate and what a Predicated instruction is
* Be able to explain pros / cons of Predication
  * Pro: no more branch so no costly pipeline flushes due to misprediction
  * Pro: control dependencies are converted to data dependencies -> much easier for compiler scheduling (explained more with VLIWs)
  * Con: both directions of the branch are fetched into the pipeline wasting resources

## VLIW Processors

* Be able to identify which component of performance what wider processors improve: CPI (or IPC)
* Be able to identify various additional structural hazards that can occur due to the wider execution
  * Extra Memory / Register File read / write ports
  * Extra EX and MEM functional units
* Be able to differentiate between VLIWs and Superscalars
* Be able to create a schedule of VLIW instructions given a sequence of operations with hazards
* Be able to create an optimized VLIW schedule with reordering
* Be able to explain why VLIW processors are power efficient
  * No hazard detection unit
  * No dynamic scheduler
  * Simpler control logic due to not having to deal with multiple instructions in each stage (VLIW only has one instruction)
* Be able to explain why VLIW code is not portable and solutions
  * Bundles not instructions
  * Binary translation
* Be able to explain how a VLIW compiler deals with control dependencies when scheduling
  * Predication
  * Loop unrolling
* Be able to explain how a VLIW compiler deals with data dependencies when scheduling
  * Remove WAR and WAW dependencies through register renaming and Static Single Assignment
  * Optimize RAW dependencies by reordering through list scheduling
* Be able to calculate the critical path through a data dependence graph which is the minimal length of the schedule

## Superscalars and Exceptions

* Be able to differentiate between in-order and out-of-order superscalars
* Be able to explain the role of the instruction queue
  * Provides a large instruction window within which processor can do dynamic scheduling
  * Keeps track of which operands and instructions are ready to issue
  * Commit instructions in program order once they are completed
* Be able to identify what types of reorderings are allowed and disallowed
  * RAW - disallowed
  * WAR, WAW - allowed with the help of the register renamer
  * RAR - always allowed to begin with
* Be able to convert code with architectural registers to code with physical registers once it passes through register renamer
* Be able to discuss the limits in IPC for Superscalars (and VLIWs)
  * Fundamental limit: limit in ILP
  * Practical limits:- limits in ILP that can be extracted practically, limits due to increases in cycle time
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
