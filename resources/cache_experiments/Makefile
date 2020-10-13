TARGETS = linked-list linked-list_nodata array array_nodata

GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot
CC = gcc
COPT = -pthread -std=c99
LOPT = -pthread
SIZES = 1 5 10 50 100 500 1000 5000 10000 50000 100000 500000 1000000
PERF_DATA := $(foreach target,$(TARGETS),$(foreach size, $(SIZES), outputs/$(target).$(size).perf))
MEM_DATA := $(foreach target,$(TARGETS),$(foreach size, $(SIZES), outputs/$(target).$(size).mem))
PLOTS := IPC.pdf MemStalls.pdf

all: $(TARGETS) $(PERF_DATA) $(MEM_DATA) $(PLOTS)

%.o: %.c
	$(CC) $(COPT) -c $<

IPC.pdf: IPC.dat generate_perf_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_perf_plot.plt

IPC.dat: $(PERF_DATA) generate_perf_plot.py
	python generate_perf_plot.py -i outputs -o $@

MemStalls.pdf: MemStalls.dat generate_memstalls_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_memstalls_plot.plt

MemStalls.dat: $(MEM_DATA) generate_memstalls_plot.py
	python generate_memstalls_plot.py -i outputs -o $@

linked-list: linked-list.o
	$(CC) $(LOPT) $^ -o $@
linked-list_nodata: linked-list_nodata.o
	$(CC) $(LOPT) $^ -o $@
array: array.o
	$(CC) $(LOPT) $^ -o $@
array_nodata: array_nodata.o
	$(CC) $(LOPT) $^ -o $@

define perf_rules
outputs/$(1).$(2).perf: $(1)
	perf record -s -e cycles,instructions ./$(1) $(2)
	perf report -T -i perf.data > $$@
	rm -f perf.data
endef

$(foreach target,$(TARGETS),$(foreach size, $(SIZES), $(eval $(call perf_rules,$(target),$(size)))))

define mem_rules
outputs/$(1).$(2).mem: $(1)
	perf record -s -e cycles,cycle_activity.stalls_mem_any,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses ./$(1) $(2)
	perf report -T -i perf.data > $$@
	rm -f perf.data
endef

$(foreach target,$(TARGETS),$(foreach size, $(SIZES), $(eval $(call mem_rules,$(target),$(size)))))

clean:
	rm -f $(TARGETS) $(PERF_DATA) $(MEM_DATA) *.pdf *.dat *.o
