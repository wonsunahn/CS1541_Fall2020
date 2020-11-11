#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <glib/gprintf.h>
#include "MemObj.h"

typedef struct
{
  // pointer to config file
  GKeyFile *keyfile;
  // result of parsed configuration
  int pipelineWidth;
  MemObj *instSource;
  MemObj *dataSource;
} Config;

int parse_config(const char *config_file_name);
void free_config();

extern Config *config;
extern bool verbose;
extern bool debug;

#endif /* #define CONFIG_H */
