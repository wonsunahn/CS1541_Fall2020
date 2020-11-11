#ifndef CACHELINE_H
#define CACHELINE_H

#include <stdint.h>
#include <string>

class CacheLine {
private:
  uint32_t tag;
  bool valid;
  bool dirty;
  uint32_t age;
public:
  CacheLine() {
    initialize();
  }

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

  std::string toString() {
    return "tag=" + std::to_string(tag) + ":valid=" + std::to_string(valid) + ":dirty=" + std::to_string(dirty) + ":age=" + std::to_string(age);
  }
};

#endif
