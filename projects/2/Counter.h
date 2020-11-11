#ifndef GSTATSD_H
#define GSTATSD_H

#include <string>

/** @brief A counter pure and simple.
*/
class Counter {
  protected:
    /** The name of the counter that gets printed on screen */
    std::string name;
    /** The value of the counter */
    long long data;

  public:
    /** Constructor.
     *
     * @param s - The name of the counter
     */
    Counter(const char *s) {
      name = s;
      data = 0;
    }

    void add(const int32_t v) {
      data += v;
    }
    void inc() {
      data++;
    }
    void dec() {
      data--;
    }

    long long getValue() const { return data; }

    std::string toString() const {
      return name + "=" + std::to_string(data);
    }
};

#endif   // GSTATSD_H
