#include <stdio.h>
#include <string.h>
#include <set>

#include "MemObj.h"
#include "Cache.h"
#include "DRAM.h"

std::map<std::string, MemObj*> MemObj::memObjs;

MemObj *MemObj::create(const char *name)
{
  GError *error = NULL;
  gchar* deviceType = NULL;
  gchar* writePolicy= NULL;

  assert(config && config->keyfile);
  assert(name);

  // If memory object already created, just return that one
  std::map<std::string, MemObj*>::iterator it = memObjs.find(name);
  if(it != memObjs.end()) {
    MemObj *obj = it->second;
    assert(obj);
    return obj;
  }

  // Create memory object according to device type and write policy
  MemObj *obj = NULL;
  deviceType = g_key_file_get_string(config->keyfile, name, "deviceType", &error);
  if(error != NULL) g_error (error->message);
  if(!strcmp(deviceType, "dram")) {
    obj = new DRAM(name);
  } else if(!strcmp(deviceType, "cache")) {
    writePolicy = g_key_file_get_string(config->keyfile, name, "writePolicy", &error);
    if(error != NULL) g_error (error->message);
    if(!strcmp(writePolicy, "WB")) {
      obj = new WBCache(name);
    } else if(!strcmp(writePolicy, "WT")) {
      obj = new WTCache(name);
    } else {
      assert(0);
    }
  } else {
    assert(0);
  }
  assert(obj);

  // Register memory object to map
  memObjs[name] = obj;

  g_free(deviceType);
  g_free(writePolicy);

  return obj;
}

MemObj::MemObj(const char *s)
  :name(s)
{
  GError *error = NULL;

  // If there is a lower level memory object, recursively create that one too!
  lowerLevel = g_key_file_get_string(config->keyfile, s, "lowerLevel", &error);
  if(error != NULL) g_error (error->message);
  if(strcmp(lowerLevel, "null")) {
    // If not "null" create that object
    lowerLevelMemObj = MemObj::create(lowerLevel);
    assert(lowerLevelMemObj);
  }
}

MemObj::~MemObj()
{
  g_free(lowerLevel);
}

void MemObj::freeAll()
{
  std::map<std::string, MemObj*>::iterator it;
  for(it = memObjs.begin(); it != memObjs.end(); it++) {
    MemObj *obj = it->second;
    delete obj;
  }
  memObjs.clear();
}

void MemObj::printAll()
{
  printf("======================================================================\n\n");
  printf("Printing all memory objects ... \n\n");
  std::map<std::string, MemObj*>::iterator it;
  for(it = memObjs.begin(); it != memObjs.end(); it++) {
    MemObj *obj = it->second;
    printf("%s\n", obj->toString().c_str());
  }
  printf("======================================================================\n");
}

void MemObj::printAllStats()
{
  printf("======================================================================\n\n");
  printf("Printing all memory stats ... \n\n");
  std::map<std::string, MemObj*>::iterator it;
  for(it = memObjs.begin(); it != memObjs.end(); it++) {
    MemObj *obj = it->second;
    printf("%s\n", obj->getStatString().c_str());
  }
  printf("\n======================================================================\n");
}

void MemObj::printAllContents()
{
  printf("======================================================================\n");
  printf("Printing all cache contents ...\n");
  std::map<std::string, MemObj*>::iterator it;
  for(it = memObjs.begin(); it != memObjs.end(); it++) {
    MemObj *obj = it->second;
    printf("%s", obj->getContentString().c_str());
  }
  printf("======================================================================\n");
}
