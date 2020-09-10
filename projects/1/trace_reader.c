/** Code by @author Wonsun Ahn
 * 
 * Utility program to read and print out the contents of a trace file in human
 * readable format.  Takes as argument the name of the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "trace.h" 

int main(int argc, char **argv)
{
  instruction *tr_entry = (instruction *) malloc(sizeof(instruction));
  size_t size;
  char *trace_file_name;
  
  if (argc == 1) {
    fprintf(stdout, "\nMissing argument: the name of the file to be read\n");
    exit(0);
  }
  trace_file_name = argv[1];
  trace_fd = fopen(trace_file_name, "rb");
  trace_init();
  while(1) {
    size = trace_get_item(&tr_entry);
   
    if (!size) 
      break; 

    // Display the generated trace 
    switch(tr_entry->type) {
        case ti_NOP:
          printf("NOP: \n") ;
          break;
        case ti_RTYPE: /* registers are translated for printing by subtracting offset  */
          printf("RTYPE: ");
		  printf("(PC: %d)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
          break;
        case ti_LOAD:      
          printf("LOAD: ");
		  printf("(PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_STORE:    
          printf("STORE: ");
		  printf("(PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_BRANCH:
          printf("BRANCH: ");
		  printf("(PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
    }
  }

  trace_uninit();

  exit(0);
}


