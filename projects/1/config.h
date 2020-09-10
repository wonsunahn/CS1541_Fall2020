#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <glib/gprintf.h>

typedef struct
{
  int pipelineWidth;
  bool splitCaches;
  int regFileWritePorts;
  bool enableForwarding;
  bool branchPredictor;
  bool branchTargetBuffer;
} Config;

int parse_config(const char *config_file_name);

extern Config *config;
extern bool verbose;
extern bool debug;

#endif /* #define CONFIG_H */
