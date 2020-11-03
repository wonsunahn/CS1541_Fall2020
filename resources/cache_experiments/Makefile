TARGETS = linked-list linked-list_nodata array array_nodata

GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot
CC = g++
COPT = -pthread
LOPT = -pthread
SIZES = 1 5 10 50 100 500 1000 5000 10000 50000 100000 500000 1000000 5000000 10000000
#SIZES = 1 5 10 50 100 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 5000 10000 50000 100000 500000 1000000 5000000 10000000
PERF_DATA := $(foreach target,$(TARGETS),$(foreach size, $(SIZES), outputs/$(target).$(size).perf))
CACHE_DATA := $(foreach target,$(TARGETS),$(foreach size, $(SIZES), outputs/$(target).$(size).cache))
CACHE_PLOTS := $(foreach target,$(TARGETS), $(target).cache.pdf)
PLOTS := CPI.pdf MemStalls.pdf $(CACHE_PLOTS)

all: $(TARGETS) $(PLOTS)

build: $(TARGETS)

.PRECIOUS: $(foreach target,$(TARGETS), $(target).cache.dat) $(PERF_DATA) $(CACHE_DATA)

CPI.pdf: CPI.dat generate_perf_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_perf_plot.plt

CPI.dat: $(PERF_DATA) generate_perf_plot.py
	python generate_perf_plot.py -i outputs -o $@

MemStalls.pdf: MemStalls.dat generate_memstalls_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_memstalls_plot.plt

MemStalls.dat: $(PERF_DATA) generate_memstalls_plot.py
	python generate_memstalls_plot.py -i outputs -o $@

%.cache.pdf: %.cache.dat generate_cachestats_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_cachestats_plot.plt

%.cache.dat: $(CACHE_DATA) generate_cachestats_plot.py
	python generate_cachestats_plot.py -i outputs -o $@ -t $*

%.o: %.c
	$(CC) $(COPT) -c $<

define link_rules
$(1): $(1).o
	$(CC) $(LOPT) $$^ -o $$@
endef

$(foreach target,$(TARGETS), $(eval $(call link_rules,$(target))))

define perf_rules
outputs/$(1).$(2).perf: $(1)
	perf record -s -e cycles,instructions,cycle_activity.stalls_mem_any ./$(1) $(2)
	perf report -T -i perf.data > $$@
	rm -f perf.data
endef

$(foreach target,$(TARGETS),$(foreach size, $(SIZES), $(eval $(call perf_rules,$(target),$(size)))))

define cache_rules
outputs/$(1).$(2).cache: $(1)
	perf record -s -e L1-dcache-loads,L1-dcache-load-misses,l2_rqsts.demand_data_rd_hit,l2_rqsts.demand_data_rd_miss,LLC-loads,LLC-load-misses ./$(1) $(2)
	perf report -T -i perf.data > $$@
	rm -f perf.data
endef

$(foreach target,$(TARGETS),$(foreach size, $(SIZES), $(eval $(call cache_rules,$(target),$(size)))))

clean:
	rm -f $(TARGETS) $(PERF_DATA) $(CACHE_DATA) *.pdf *.dat *.o
