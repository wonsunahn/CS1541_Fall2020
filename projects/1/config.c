/** Code by @author Wonsun Ahn
 * 
 * Functions used to parse and read in the processor configuration file.
 */

#include <assert.h>
#include "config.h"

Config *config;
bool verbose = false;
bool debug = false;

int parse_config(const char *config_file_name)
{
  GKeyFile *keyfile;
  GError *error = NULL;

  /* Create a new GKeyFile object and a bitwise list of flags. */
  keyfile = g_key_file_new ();

  /* Load the GKeyFile from keyfile.conf or return. */
  if (!g_key_file_load_from_file (keyfile, config_file_name, G_KEY_FILE_NONE, &error))
  {
    g_error (error->message);
    return 0;
  }

  config = g_slice_new (Config);

  config->pipelineWidth = g_key_file_get_integer (keyfile, "pipeline", "width", NULL);
  assert(config->pipelineWidth <= 2);  // fetch width > 2 is not supported

  config->splitCaches = g_key_file_get_boolean (keyfile, "structural hazard", "splitCaches", NULL);
  config->regFileWritePorts = g_key_file_get_integer (keyfile, "structural hazard", "regFileWritePorts", NULL);
  config->enableForwarding = g_key_file_get_boolean (keyfile, "data hazard", "enableForwarding", NULL);
  config->branchPredictor = g_key_file_get_boolean (keyfile, "control hazard", "branchPredictor", NULL);
  config->branchTargetBuffer = g_key_file_get_boolean (keyfile, "control hazard", "branchTargetBuffer", NULL);

  return 1;
}
