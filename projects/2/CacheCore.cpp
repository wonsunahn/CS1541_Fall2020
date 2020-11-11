#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "CacheCore.h"

CacheCore::CacheCore(uint32_t s, uint32_t a, uint32_t b, const char *pStr)
  : size(s)
  ,lineSize(b)
  ,assoc(a)
  ,numLines(s/b)
{
  if (strcasecmp(pStr, "RANDOM") == 0)
    policy = RANDOM;
  else if (strcasecmp(pStr, "LRU") == 0)
    policy = LRU;
  else {
    assert(0);
  }

  content = new CacheLine[numLines + 1];

  for(uint32_t i = 0; i < numLines; i++) {
    content[i].initialize();
  }
}

CacheCore::~CacheCore() {
  delete [] content;
}

CacheLine *CacheCore::accessLine(uint32_t addr)
{
  // TODO: Implement
  return NULL;
}

CacheLine *CacheCore::allocateLine(uint32_t addr, uint32_t *rplcAddr) {
  // TODO: Implement
  return NULL;
}
