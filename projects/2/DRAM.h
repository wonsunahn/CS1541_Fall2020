#ifndef DRAM_H
#define DRAM_H

#include <glib.h>
#include "config.h"

/** @brief A DRAM memory.
 *
 * DRAM memory is like a cache that always hits.  Enough said.
 */
class DRAM : public MemObj
{
  protected:
    uint32_t hitDelay;

    Counter readHits;
    Counter writeHits;

  public:
    /** Constructor.  First invokes the parent MemObj constructor then
     * initializes hitDelay from the config file.
     *
     * @param name - The name of the cache object on the config file.
     */
    DRAM(const char *name)
      : MemObj(name)
        ,readHits("readHits")
        ,writeHits("writeHits")
    {
      GError *error = NULL;
      // Get hit delay from config file
      hitDelay = g_key_file_get_integer(config->keyfile, name, "hitDelay", NULL);
      if(error != NULL) g_error (error->message);
    }

    ~DRAM()
    {
    }

    /** Accesses memory object with memory request.  Adds the hitDelay to
     * the memory request latency and updates statistics depending on read, write, or writeBack.
     *
     * @param mreq - The memory request
     */
    void access(MemRequest *mreq) {
      mreq->addLatency(hitDelay);

      if(verbose) {
        const char *memOp;
        switch(mreq->getMemOperation()){
          case MemRead: memOp = "MemRead"; break;
          case MemWrite: memOp = "MemWrite"; break;
          case MemWriteBack: memOp = "MemWriteBack"; break;
          default: assert(0);
        }
        printf("%s->access(%s, addr: %u, latency: %u)\n", getName().c_str(), memOp, mreq->getAddr(), mreq->getLatency());
      }

      switch(mreq->getMemOperation()){
        case MemRead:
          readHits.inc();
          break;
        case MemWrite:
          writeHits.inc();
          break;
        case MemWriteBack:
          break;
        default:
          assert(0);
          break;
      }
    }

    /** Returns a string that describes the DRAM */
    std::string toString() const {
      std::string ret;
      ret += "[" + getName() + "]\n";
      ret += "device type = dram\n";
      ret += "hit time = " + std::to_string(hitDelay) + "\n";
      return ret;
    }

    /** Returns a string that summarizes access statistics */
    std::string getStatString() const {
      std::string ret;
      ret += getName() + ":";
      ret += readHits.toString() + ":";
      ret += writeHits.toString();
      return ret;
    }

    /** Returns an empty string.  Meaningful only for cache objects. */
    std::string getContentString() const {
      return "";
    }
};

#endif
