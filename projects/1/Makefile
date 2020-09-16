TARGETS = five_stage trace_reader trace_generator
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)
CONFS = $(wildcard confs/*.conf)
TRACES = $(wildcard traces/*.tr)
OUTPUTS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
OUTPUTS_SOLUTION := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), outputs_solution/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).out))
DIFFS := $(foreach conf,$(CONFS),$(foreach trace, $(TRACES), diffs/$(trace:traces/%.tr=%).$(conf:confs/%.conf=%).diff))

COPT = -g -Wall -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/
LOPT = -lglib-2.0
CC = g++

all: build run
build: $(TARGETS)
run: $(OUTPUTS) $(OUTPUTS_SOLUTION) $(DIFFS)

five_stage.o: CPU.h config.h
config.o: config.h
CPU.o: CPU.h config.h trace.h
trace_reader.o: CPU.h trace.h

five_stage: five_stage.o config.o CPU.o trace.o
	$(CC) $(LOPT) $^ -o $@

trace_reader: trace_reader.o config.o CPU.o trace.o
	$(CC) $(LOPT) $^ -o $@

trace_generator: trace_generator.o config.o CPU.o trace.o
	$(CC) $(LOPT) $^ -o $@

%.o: %.c
	$(CC) -c $(COPT) $<

define run_rules
outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage $(1) $(2)
	@echo "Running ./five_stage -t $(1) -c $(2) -d > $$@"
	-@./five_stage -t $(1) -c $(2) -d > $$@

outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out: five_stage_solution $(1) $(2)
	@echo "Running ./five_stage_solution -t $(1) -c $(2) -d > $$@"
	-@./five_stage_solution -t $(1) -c $(2) -d > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call run_rules,$(trace),$(conf)))))

define diff_rules
diffs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).diff: outputs/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out
	@echo "Running diff -dwy -W 170 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@"
	-@diff -dwy -W 170 $$< outputs_solution/$(1:traces/%.tr=%).$(2:confs/%.conf=%).out > $$@
endef

$(foreach trace,$(TRACES),$(foreach conf, $(CONFS), $(eval $(call diff_rules,$(trace),$(conf)))))

clean:
	rm -f $(TARGETS) $(OBJECTS) $(OUTPUTS) $(DIFFS)
