/** Code by @author Wonsun Ahn
 * 
 * Main function. Parses commandline arguments and invokes the five stages in
 * CPU.c at every clock cycle.
 */

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "trace.h" 

void print_usage_info()
{
  printf("USAGE: five_stage [OPTIONS]\n");
  printf("Runs a CPU simulation given a CPU configuration file and an instruction trace file.\n\n");
  printf("  -h           this help screen.\n");
  printf("  -v           verbose output (shows each instruction).\n");
  printf("  -d           debug output (shows pipeline on each cycle).\n");
  printf("  -c file      [Required] uses file as configuration file.\n");
  printf("  -t file      [Required] uses file as input trace file.\n");
}

int main(int argc, char **argv)
{
  char *trace_file_name = NULL;
  char *config_file_name = NULL;
  
  char c;
  while ((c = getopt (argc, argv, "hvdc:t:")) != -1) {
    switch (c) {
      case 'h':
        print_usage_info();
        return 0;
      case 'v':
        verbose = true;
        break;
      case 'd':
        debug = true;
        verbose = true;
        break;
      case 't':
        trace_file_name = optarg;
        break;
      case 'c':
        config_file_name = optarg;
        break;
      case '?':
        if (optopt == 't' || optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
    }
  }

  if (config_file_name == NULL || trace_file_name == NULL) {
    print_usage_info();
    exit(1);
  }

  if (!parse_config(config_file_name)) {
    fprintf(stderr, "\nError while parsing config file %s.\n\n", trace_file_name);
    exit(1);
  }

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stderr, "\nError while opening trace file %s.\n\n", trace_file_name);
    exit(1);
  }

  trace_init();

  while(1) {
    /* move the pipeline forward */
    cycle_number++;

    if (debug) {/* print cycle number if debug=1 */
      printf("[CYCLE NUMBER: %d]\n", cycle_number);
    }

    /* move instructions one stage ahead */
    writeback();
    memory();
    issue();
    decode();
    fetch();

    if (debug) {/* print the pipeline contents if debug=1 */
      print_pipeline();
    }

    if (is_finished()) {
      /* all instructions simulated to completion */
      printf("+ Number of cycles : %u\n", cycle_number);
      printf("+ IPC (Instructions Per Cycle) : %0.4f\n", (float)inst_number / (float)cycle_number);
      break;
    }
  }

  trace_uninit();

  return 0;
}


