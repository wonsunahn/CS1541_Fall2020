#ifndef CACHE_H
#define CACHE_H

#include <queue>

#include "CacheCore.h"
#include "Counter.h"
#include "MemObj.h"
#include "MemRequest.h"

/** @brief A generic cache.
 *
 * Provides only abstract interfaces for read, write, and writeBack methods
 * so can't be instantiated.  Children classes WBCache and WTCache override
 * these methods to implement them respectively.
 */
class Cache: public MemObj
{
  protected:
    /** The cache block array for this cache */
    CacheCore *cacheCore;
    /** The hit time in clock cycles */
    uint32_t hitDelay;

    // BEGIN Statistics
    Counter readHits;
    Counter readMisses;
    Counter writeHits;
    Counter writeMisses;
    Counter writeBacks;
    // END Statistics

    /** Handler for read memory requests.
     *
     * @param mreq - The memory request
     */
    virtual void read(MemRequest *mreq) = 0;
    /** Handler for write memory requests.
     *
     * @param mreq - The memory request
     */
    virtual void write(MemRequest *mreq) = 0;
    /** Handler for write back memory requests.
     *
     * @param mreq - The memory request
     */
    virtual void writeBack(MemRequest *mreq) = 0;

  public:
    /** Constructor.  First invokes the parent MemObj constructor then reads
     * in various parameters from the config file and uses them to
     * initialize hitDelay and cacheCore.
     *
     * @param name - The name of the cache object on the config file.
     */
    Cache(const char *name);
    
    /** Destructor. */
    virtual ~Cache();

    /** Returns the write policy, either "WB" or "WT" */
    virtual std::string getWritePolicy() const = 0;

    /** Accesses memory object with memory request.  Adds the hitDelay to
     * the memory request latency and calls read, write, or writeBack
     * depending on the memory request type.
     *
     * @param mreq - The memory request
     */
    void access(MemRequest *mreq);
    /** Returns a string that describes the cache */
    std::string toString() const;
    /** Returns a string that summarizes access statistics */
    std::string getStatString() const;
    /** Returns a string that dumps all valid lines in cache */
    std::string getContentString() const;
};

/** @brief <B>TODO</B>: A write back cache.
 *
 * On a write request, it marks the cache block dirty and only writes back
 * the dirty block to lower level memory on displacements.  On a write miss,
 * it allocates a new block, reads it in from lower memory, then marks it
 * dirty (write allocate).
 */
class WBCache : public Cache {
  protected:
    /** <B>TODO</B>: Handler for read memory requests.  On a read hit, it
     * returns immediately.  On a read miss, first it passes on the memory
     * request to lower level memory and when the request returns, it
     * allocates a new cache block for it.  If the allocation displaces a
     * cache block and that cache block is dirty, it generates a new write
     * back memory request to update lower memory.  Don't forget to delete
     * the write back memory request or else you may have memory leaks!
     *
     * @param mreq - The memory request
     */
    void read(MemRequest *mreq);

    /** <B>TODO</B>: Handler for write memory requests.  On a write hit, it
     * returns immediately after marking the block dirty.  On a write miss,
     * first it passes on the memory request to lower level memory, after
     * changing to a read request.  We need to read the cache block from
     * lower memory in order to fill the block with valid data on
     * allocation.  When the request returns, a new cache block is
     * allocated.  If the allocation displaces a cache block and that cache
     * block is dirty, it generates a new write back memory request to
     * update lower memory.  Don't forget to delete the write back memory
     * request or else you may have memory leaks!
     *
     * @param mreq - The memory request
     */
    void write(MemRequest *mreq);

    /** <B>TODO</B>: Handler for write back memory requests.  On a hit, the
     * write back can be handled by this cache and you need to search no
     * further.  On a miss, you need to keep on searching lower level memory
     * to send the dirty block.
     *
     * @param mreq - The memory request
     */
    void writeBack(MemRequest *mreq);

  public:
    WBCache(const char *name);
    ~WBCache();

    std::string getWritePolicy() const { return "WB"; }
};

/** @brief <B>TODO</B>: A write through cache.
 *
 * On a write request, it updates the cache block if it is a hit.  If it is
 * a miss, then no block is allocated (no write allocate).  Regardless of
 * hit or miss, the write is propagated to lower level memory.  There are no
 * dirty blocks with write through sicne all blocks are kept consistent at
 * all times.
 */
class WTCache : public Cache { protected:
    /** <B>TODO</B>: Handler for read memory requests.  On a read hit, it
     * returns immediately.  On a read miss, first it passes on the memory
     * request to lower level memory and when the request returns, it
     * allocates a new cache block for it.  There are no dirty blocks in a
     * write through cache, there is no need to write back anything on a
     * block displacement due to the allocation.
     *
     * @param mreq - The memory request
     */
    void read(MemRequest *mreq);

    /** <B>TODO</B>: Handler for write memory requests.  It propagates the
     * write to lower level memory regardless of hit or miss.
     *
     * @param mreq - The memory request
     */
    void write(MemRequest *mreq);

    /** Handler for write back memory requests.  This is only needed if you
     * have a write back cache above the write through cache.  No sane
     * architect would design a CPU that way, so no need to implement.  All
     * that is needed is an assert(0) to make sure this is never called.
     *
     * @param mreq - The memory request
     */
    void writeBack(MemRequest *mreq);

  public:
    WTCache(const char *name);
    ~WTCache();

    std::string getWritePolicy() const { return "WT"; }
};

#endif
