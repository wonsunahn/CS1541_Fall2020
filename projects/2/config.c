/** Code by @author Wonsun Ahn
 * 
 * Functions used to parse and read in the processor configuration file.
 */

#include <stdio.h>
#include <assert.h>
#include "config.h"

Config *config;
bool verbose = false;
bool debug = false;

int parse_config(const char *config_file_name)
{
  gchar *instSource = NULL;
  gchar *dataSource = NULL;
  GError *error = NULL;

  config = g_slice_new (Config);

  /* Create a new GKeyFile object and a bitwise list of flags. */
  config->keyfile = g_key_file_new ();

  /* Load the GKeyFile from keyfile.conf or return. */
  if (!g_key_file_load_from_file (config->keyfile, config_file_name, G_KEY_FILE_NONE, &error))
  {
    g_error (error->message);
    g_slice_free(Config, config);
    return 0;
  }

  config->pipelineWidth = g_key_file_get_integer (config->keyfile, "pipeline", "width", NULL);
  if(error != NULL) g_error (error->message);
  assert(config->pipelineWidth <= 2);  // fetch width > 2 is not supported

  instSource = g_key_file_get_string(config->keyfile, "pipeline", "instSource", NULL);
  if(error != NULL) g_error (error->message);
  dataSource = g_key_file_get_string(config->keyfile, "pipeline", "dataSource", NULL);
  if(error != NULL) g_error (error->message);

  config->instSource = MemObj::create(instSource);
  config->dataSource = MemObj::create(dataSource);

  printf("Memory system setup successful.\n");
  MemObj::printAll();

  g_free(instSource);
  g_free(dataSource);
  return 1;
}

void free_config()
{
  assert(config && config->keyfile);

  g_key_file_free(config->keyfile);
  g_slice_free(Config, config);
}
