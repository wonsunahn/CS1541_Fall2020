TARGETS = devicequery max_blocks_per_sm mat_vec_gpu

GNUPLOT = /afs/cs.pitt.edu/courses/1541/gnuplot-5.2.8/bin/gnuplot
CC = gcc
COPT = 
NVCC = /opt/cuda-7.0/bin/nvcc
NVPROF = /opt/cuda-7.0/bin/nvprof -m l1_cache_global_hit_rate -m l2_l1_read_hit_rate -m l2_read_throughput
NVCOPT = 

# Temporary hack because I don't have root permissions to install Qt5
LIBQT5 = /afs/cs.pitt.edu/courses/1541/libQt5/
export LD_LIBRARY_PATH=$(LIBQT5)

POLICIES = 1 2 3
MATVEC_BLOCK_SIZES = 1 2 4 8 16 32 64 128 256 512 1024
MATVEC_DATA := $(foreach policy,$(POLICIES),$(foreach size, $(MATVEC_BLOCK_SIZES), outputs/mat_vec_gpu.$(policy).$(size).perf))
MATVEC_PROFILE := $(foreach size, $(MATVEC_BLOCK_SIZES), outputs/mat_vec_gpu.2.$(size).prof)
MATMUL_BLOCK_SIZES = 1 2 4 8 16 32 
MATMUL_DATA := $(foreach policy,$(POLICIES),$(foreach size, $(MATMUL_BLOCK_SIZES), outputs/mat_mul_gpu.$(policy).$(size).perf))
PLOTS := MatVecTime.pdf MatVecProfile.pdf MatMulTime.pdf MatMulTimeZoomIn.pdf

all: build plots
build: $(TARGETS)
plots: $(MATVEC_DATA) $(MATMUL_DATA) $(MATVEC_PROFILE) $(PLOTS)

MatVecTime.pdf: MatVecTime.dat generate_mat_vec_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_mat_vec_plot.plt

MatVecTime.dat: $(MATVEC_DATA) generate_plot.py
	python generate_plot.py -i outputs -p mat_vec_gpu -o $@

MatVecProfile.pdf: MatVecProfile.dat generate_mat_vec_profile_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_mat_vec_profile_plot.plt

MatVecProfile.dat: $(MATVEC_PROFILE) generate_mat_vec_profile_plot.py
	python generate_mat_vec_profile_plot.py -i outputs -p mat_vec_gpu -o $@

MatMulTime.pdf: MatMulTime.dat generate_mat_mul_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_mat_mul_plot.plt

MatMulTimeZoomIn.pdf: MatMulTime.dat generate_mat_mul_zoomin_plot.plt
	$(GNUPLOT) -e "inputFile='$<'" -e "outputFile='$@'" generate_mat_mul_zoomin_plot.plt

MatMulTime.dat: $(MATMUL_DATA) generate_plot.py
	python generate_plot.py -i outputs -p mat_mul_gpu -o $@

%: %.c
	$(CC) $(COPT) $< -o $@

%: %.cu
	$(NVCC) $(NVCOPT) $< -o $@

define matvec_rules
outputs/mat_vec_gpu.$(1).$(2).perf: mat_vec_gpu
	./mat_vec_gpu 8192 $(2) $(1) nodebug > $$@
endef

$(foreach policy,$(POLICIES),$(foreach size, $(MATVEC_BLOCK_SIZES), $(eval $(call matvec_rules,$(policy),$(size)))))

define matvec_profile_rules
outputs/mat_vec_gpu.2.$(1).prof: mat_vec_gpu
	$(NVPROF) ./mat_vec_gpu 8192 $(1) 2 nodebug > /dev/null 2> $$@
endef

$(foreach size, $(MATVEC_BLOCK_SIZES), $(eval $(call matvec_profile_rules,$(size))))

outputs/mat_mul_gpu.1.perf: mat_mul_gpu
	./mat_mul_gpu 1024 1 1 nodebug > $@

define matmul_cpu_rules
outputs/mat_mul_gpu.1.$(1).perf: outputs/mat_mul_gpu.1.perf
	cp $$< $$@
endef

$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_cpu_rules,$(size))))

define matmul_rules
outputs/mat_mul_gpu.$(1).$(2).perf: mat_mul_gpu
	./mat_mul_gpu 1024 $(2) $(1) nodebug > $$@
endef

$(foreach policy,2 3,$(foreach size, $(MATMUL_BLOCK_SIZES), $(eval $(call matmul_rules,$(policy),$(size)))))

clean:
	rm -f $(TARGETS) *.pdf *.dat outputs/*.perf outputs/*.prof
