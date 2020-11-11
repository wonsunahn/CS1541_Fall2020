#ifndef TRACE_H
#define TRACE_H

#include "config.h"
#include "CPU.h"

#define TRACE_BUFSIZE 1024*1024

enum Format {
  SHORT_FORM,
  LONG_FORM,
  ADDR_ONLY
};

/* Trace related functions */
extern FILE *trace_fd;
void trace_init();
void trace_uninit();
int trace_get_item(instruction **item);
int write_trace(instruction item, char *fname);
char* get_instruction_string(dynamic_inst dinst, Format format);

#endif /* #define TRACE_H */
