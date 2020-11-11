#ifndef CACHELINE_H
#define CACHELINE_H

#include <stdint.h>
#include <string>

/** @brief A cache line (a.k.a cache block).
 *
 * Contains a tag, valid bit, dirty bit (for write-back caches), and age
 * (for LRU cache block replacement).
 */
class CacheLine {
  protected:
    /** Tag for cache block */
    uint32_t tag;
    /** Valid bit */
    bool valid;
    /** Dirty bit */
    bool dirty;
    /** Age (32 bits in the counter, which is unrealistic but...) */
    uint32_t age;
  public:
    /** Constructor.  Initializes block. */
    CacheLine() {
      initialize();
    }

    /** Clears cache block for new allocation */
    void initialize() {
      tag = 0;
      valid = false;
      dirty = false;
      age = 0;
    }

    uint32_t getTag() const { return tag; }
    void setTag(uint32_t a) { tag = a; }

    bool isDirty() const { return dirty; }
    void makeDirty() { dirty = true; }

    bool isValid() const { return valid; }
    void validate() { valid = true; }

    uint32_t getAge() const { return age; }
    void incAge() { age++; }
    void resetAge() { age = 0; }

    /** Returns string representation of the cache block. */
    std::string toString() {
      return "tag=" + std::to_string(tag) + ":valid=" + std::to_string(valid) + ":dirty=" + std::to_string(dirty) + ":age=" + std::to_string(age);
    }
};

#endif
