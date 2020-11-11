/** Code by @author Wonsun Ahn
 * 
 * Functions to read and write the trace file.
 */

#include <inttypes.h>
#include <assert.h>
#include "trace.h"
#include "CPU.h"

FILE *trace_fd;
static int trace_buf_ptr;
static int trace_buf_end;
static instruction *trace_buf;
static FILE *out_fd;

int is_big_endian(void)
{
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;
}

uint32_t my_ntohl(uint32_t x)
{
	unsigned char *s = (unsigned char *)&x;
	return (uint32_t)(s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0]);
}

void trace_init()
{
	trace_buf = (instruction *) malloc(sizeof(instruction) * TRACE_BUFSIZE);

	if (!trace_buf) {
		fprintf(stdout, "** trace_buf not allocated\n");
		exit(-1);
	}

	trace_buf_ptr = 0;
	trace_buf_end = 0;
}

void trace_uninit()
{
	free(trace_buf);
	fclose(trace_fd);
}

int trace_get_item(instruction **item)
{
	int n_items;

	if (trace_buf_ptr == trace_buf_end) {	/* if no more unprocessed items in the trace buffer, get new data  */
		n_items = fread(trace_buf, sizeof(instruction), TRACE_BUFSIZE, trace_fd);
		if (!n_items) return 0;				/* if no more items in the file, we are done */

		trace_buf_ptr = 0;
		trace_buf_end = n_items;			/* n_items were read and placed in trace buffer */
	}

	*item = &trace_buf[trace_buf_ptr];	/* read a new trace item for processing */
	trace_buf_ptr++;

	if (is_big_endian()) {
		(*item)->PC = my_ntohl((*item)->PC);
		(*item)->Addr = my_ntohl((*item)->Addr);
	}

	return 1;
}

int write_trace(instruction item, char *fname)
{
	out_fd = fopen(fname, "a");
	int n_items;
	if (is_big_endian()) {
		(&item)->PC = my_ntohl((&item)->PC);
		(&item)->Addr = my_ntohl((&item)->Addr);
	}

	n_items = fwrite(&item, sizeof(instruction), 1, out_fd);
	fclose(out_fd);
	if (!n_items) return 0;				/* if no more items in the file, we are done */

		
	return 1;
}

char* get_instruction_string(dynamic_inst dinst, Format format)
{
  static char buffer[100];
  instruction inst = dinst.inst;
  const char *name;

  switch(inst.type) {
    case ti_NOP:
      name = "NOP";
      break;
    case ti_RTYPE:
      name = "RTYPE";
      break;
    case ti_ITYPE:
      name = "ITYPE";
      break;
    case ti_LOAD:
      name = "LOAD";
      break;
    case ti_STORE:
      name = "STORE";
      break;
    case ti_BRANCH:
      name = "BRANCH";
      break;
    case ti_JTYPE:
      name = "JTYPE";
      break;
    case ti_SPECIAL:
      name = "SPECIAL";
      break;
    case ti_JRTYPE:
      name = "JRTYPE";
      break;
    default:
      assert(0);
  }
  switch(format) {
    case SHORT_FORM:
      snprintf(buffer, 100, "%7s: (Seq: %8d)(Regs: %3d, %3d, %3d)", name, dinst.seq, inst.dReg, inst.sReg_a, inst.sReg_b);
      break;
    case LONG_FORM:
      snprintf(buffer, 100, "%7s: (Seq: %8d)(Regs: %3d, %3d, %3d)(Addr: %u)(PC: %u)", name, dinst.seq, inst.dReg, inst.sReg_a, inst.sReg_b, inst.Addr, inst.PC);
      break;
    case ADDR_ONLY:
      snprintf(buffer, 100, "%s: (Seq: %8d)(Addr: %u)(PC: %u)", name, dinst.seq, inst.Addr, inst.PC);
      break;
  }
  return buffer;
}

