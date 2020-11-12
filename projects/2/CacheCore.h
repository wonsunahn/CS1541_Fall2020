#ifndef CACHECORE_H
#define CACHECORE_H

#include <assert.h>
#include <string>
#include "log2i.h"
#include "CacheLine.h"

enum    ReplacementPolicy  {LRU, RANDOM};

/** @brief <B>TODO</B>: A cache block array with the given capacity, cache
 * block size, and associativity.
 *
 * It is organized into rows (sets) and columns (blocks per set).  For now,
 * it has only one replacement policy: LRU (Least Recently Used).  You may
 * find the log2i function in log2i.h helpful in calculating the number of
 * tag bits, row bits, etc.
 */
class CacheCore {

  protected:

    /** Array of cache blocks for the entire cache */
    CacheLine *content;
    /** The cache replacement policy */
    ReplacementPolicy policy;

    /** The capacity of the cache */
    const uint32_t  size;
    /** The size of one cache block */
    const uint32_t  lineSize;
    /** The associativity */
    const uint32_t  assoc;
    /** The number of cache blocks */
    const uint32_t  numLines;

  protected:

    /** Returns the capacity of the cache. */
    uint32_t  getSize() const       { return size;        }
    /** Returns the size of one cache block. */
    uint32_t  getLineSize() const   { return lineSize;    }
    /** Returns the associativity. */
    uint32_t  getAssoc() const      { return assoc;       }
    /** Returns the number of cache blcoks. */
    uint32_t  getNumLines() const   { return numLines;    }

    /** <B>TODO</B>: Returns the row for the given content array index.<br>
     * Content index = row * associativity + column<br>
     * So, for example, if index = 11 and associativity = 4, then row = 2.
     *
     * @param index - The content array index
     *
     * @return The corresponding row number
     */
    uint32_t index2Row(uint32_t index) const {
      // TODO: Implement
      return 0;
    }

    /** <B>TODO</B>: Returns the column for the given content array index.<br>
     * Content index = row * associativity + column<br>
     * So, for example, if index = 11 and associativity = 4, then column = 3.
     *
     * @param index - The content array index
     *
     * @return The corresponding column number
     */
    uint32_t index2Column(uint32_t index) const {
      // TODO: Implement
      return 0;
    }

  public:

    /** Constructor.
     *
     * @param s - The size (capacity) of the cache.
     * @param a - The associativity of the cache.
     * @param b - The cache block size.
     * @param pStr - The replacement policy (currently not used).
     */
    CacheCore(uint32_t s, uint32_t a, uint32_t b, const char *pStr);

    virtual ~CacheCore();

    /** Returns a string that describes the cache */
    std::string toString() {
      std::string ret;
      ret += "capacity = " + std::to_string(getSize()) + "\n";
      ret += "block size = " + std::to_string(getLineSize()) + "\n";
      ret += "associativity = " + std::to_string(getAssoc()) + "\n";
      return ret;
    }

    /** Returns a string that dumps all valid lines in cache */
    std::string getContentString() {
      std::string ret;
      for(uint32_t i = 0; i < getNumLines(); i++) {
        if(content[i].isValid()) {
          int row = index2Row(i);
          int column = index2Column(i);
          ret += "(" + std::to_string(row) + ", " + std::to_string(column) + ") ";
          ret += content[i].toString() + "\n";
        }
      }
      return ret;
    }

    /** <B>TODO</B>: Returns a cache block whose tag matches the address,
     * NULL otherwise.  Also, resets the age of the accessed block to 0 and
     * increments the ages of all other blocks in the same set by 1,
     * according to the LRU policy.
     *
     * @param addr - The accessed address
     *
     * @return The matching cache block, or NULL if none matching
     */
    CacheLine *accessLine(uint32_t addr);

    /** <B>TODO</B>: Returns a cache block allocated for addr.  First, the
     * set is searched for invalid blocks.  If there is one, then the first
     * invalid block is chosen for allocation.  The allocated block is
     * validated and then initialized with the given address before being
     * returned.  If there are no invalid blocks, then a valid block needs
     * to be replaced.  A block is chosen according to LRU based on the ages
     * of the blocks.  If that block is dirty, rplcAddr is updated with the
     * address of the replaced block to be used later for write back.
     *
     * @param addr - The accessed address
     * @param rplcAaddr - The address of the dirty block that is replaced
     * (if it exists), or 0 otherwise
     *
     * @return The allocated cache block
     */
    CacheLine *allocateLine(uint32_t addr, uint32_t *rplcAddr);
};

#endif // CACHECORE_H
