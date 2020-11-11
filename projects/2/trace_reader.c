/** Code by @author Wonsun Ahn
 * 
 * Utility program to read and print out the contents of a trace file in human
 * readable format.  Takes as argument the name of the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "trace.h" 

int main(int argc, char **argv)
{
  instruction *tr_entry = (instruction *) malloc(sizeof(instruction));
  size_t size;
  char *trace_file_name;
  dynamic_inst dinst = {0};
  
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
    dinst.seq++;
    dinst.inst = *tr_entry;

    printf("%s\n", get_instruction_string(dinst, LONG_FORM));
  }

  trace_uninit();

  exit(0);
}


