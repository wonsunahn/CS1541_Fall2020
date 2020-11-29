TARGETS = vec_inc_gpu 

GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot
CC = gcc
COPT = 
NVCC = /opt/cuda-7.0/bin/nvcc
NVCOPT = 
NVPROF = /opt/cuda-7.0/bin/nvprof

# Temporary hack because I don't have root permissions to install Qt5
LIBQT5 = /afs/cs.pitt.edu/courses/1541/libQt5/
export LD_LIBRARY_PATH=$(LIBQT5)

INC_TIMES = 1 100
POLICIES = 1 2
BLOCK_SIZES = 1 2 4 8 16 32 64 128 256 512 1024
DATA := $(foreach inc_times,$(INC_TIMES),$(foreach policy,$(POLICIES),$(foreach size, $(BLOCK_SIZES), outputs/vec_inc_gpu.$(inc_times).$(policy).$(size).perf)))
PLOTS := Performance.pdf 

all: build plots
build: $(TARGETS)
plots: $(DATA) $(PLOTS)

Performance.pdf: Performance.dat generate_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_plot.plt

Performance.dat: $(DATA) generate_plot.py
	python generate_plot.py -i outputs -p vec_inc_gpu -o $@

define perf_rules
outputs/vec_inc_gpu.$(1).$(2).$(3).perf: vec_inc_gpu
	./vec_inc_gpu $(1) $(3) $(2) nodebug > $$@
endef

$(foreach inc_times,$(INC_TIMES),$(foreach policy,$(POLICIES),$(foreach size, $(BLOCK_SIZES), $(eval $(call perf_rules,$(inc_times),$(policy),$(size))))))

%: %.cu
	$(NVCC) $(NVCOPT) $< -o $@

clean:
	rm -f $(TARGETS) $(DATA) $(PLOTS)
