#ifndef LOG2I_H
#define LOG2I_H

#include <stdint.h>
#include <assert.h>

/** Computes log2(n) after rounding n up to the closest power of 2 */
short log2i(uint32_t n);

#endif // LOG2I_H
