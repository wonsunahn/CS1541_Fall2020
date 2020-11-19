TARGETS = devicequery mat_mul_gpu 

GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot
CC = gcc
COPT = 
NVCC = /opt/cuda-7.0/bin/nvcc
NVCOPT = 
NVPROF = /opt/cuda-7.0/bin/nvprof

# Temporary hack because I don't have root permissions to install Qt5
LIBQT5 = /afs/cs.pitt.edu/courses/1541/libQt5/
export LD_LIBRARY_PATH=$(LIBQT5)

POLICIES = 1 2 3
MATMUL_BLOCK_SIZES = 4 8 16 32 
MATMUL_DATA := $(foreach policy,$(POLICIES),$(foreach size, $(MATMUL_BLOCK_SIZES), outputs/mat_mul_gpu.$(policy).$(size).perf))
MATMUL_OUTPUTS := $(foreach policy,1 2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), outputs/mat_mul_gpu.$(policy).$(size).out))
MATMUL_DIFFS := $(foreach policy,2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), diffs/mat_mul_gpu.$(policy).$(size).diff))
PLOTS := MatMulTime.pdf 

all: build diffs plots
build: $(TARGETS)
diffs: $(MATMUL_DIFFS)
plots: $(MATMUL_DATA) $(PLOTS)

MatMulTime.pdf: MatMulTime.dat generate_mat_mul_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_mat_mul_plot.plt

MatMulTime.dat: $(MATMUL_DATA) generate_plot.py
	python generate_plot.py -i outputs -p mat_mul_gpu -o $@

%: %.c
	$(CC) $(COPT) $< -o $@

%: %.cu
	$(NVCC) $(NVCOPT) $< -o $@

outputs/mat_mul_gpu.1.perf: mat_mul_gpu
	./mat_mul_gpu 1024 1 1 nodebug > $@

define matmul_cpu_perf_rules
outputs/mat_mul_gpu.1.$(1).perf: outputs/mat_mul_gpu.1.perf
	cp $$< $$@
endef

$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_cpu_perf_rules,$(size))))

define matmul_gpu_perf_rules
outputs/mat_mul_gpu.$(1).$(2).perf: mat_mul_gpu
	./mat_mul_gpu 1024 $(2) $(1) nodebug > $$@
endef

$(foreach policy,2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_gpu_perf_rules,$(policy),$(size)))))

outputs/mat_mul_gpu.1.out: mat_mul_gpu
	./mat_mul_gpu 1024 1 1 debug > $@

define matmul_cpu_output_rules
outputs/mat_mul_gpu.1.$(1).out: outputs/mat_mul_gpu.1.out
	cp $$< $$@
endef

$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_cpu_output_rules,$(size))))

define matmul_gpu_output_rules
outputs/mat_mul_gpu.$(1).$(2).out: mat_mul_gpu
	./mat_mul_gpu 1024 $(2) $(1) debug > $$@
endef

$(foreach policy,2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_gpu_output_rules,$(policy),$(size)))))

define matmul_diff_rules
diffs/mat_mul_gpu.$(1).$(2).diff: outputs/mat_mul_gpu.1.$(2).out outputs/mat_mul_gpu.$(1).$(2).out mat_mul_gpu
	-python diff.py outputs/mat_mul_gpu.1.$(2).out outputs/mat_mul_gpu.$(1).$(2).out > $$@
endef

$(foreach policy,2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_diff_rules,$(policy),$(size)))))

clean:
	rm -f $(TARGETS) $(PLOTS) $(MATMUL_DATA) $(MATMUL_OUTPUTS) $(MATMUL_DIFFS)
