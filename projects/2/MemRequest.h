#ifndef MEMREQUEST_H
#define MEMREQUEST_H

enum MemOperation {
  MemRead = 0,
  MemWrite,
  MemWriteBack,
};

/** @brief A memory request.
 *
 * There are three types of memory requests: MemRead, MemWrite, and
 * MemWriteBack.  The memory request conceptually travels down the memory
 * hierarchy and then up incurring latency as it visits each memory object
 * in its path.
 */
class MemRequest {
  protected:
    /** Latency incurred by the memory request */
    uint32_t latency;

    /** The address for the memory request */
    uint32_t addr;

    /** The type of memory operation for the memory request */
    MemOperation memOp;

  public:

    /** Constructor.  
     *
     * @param a - The address for the memory request.
     * @param m - The type of memory operation.
     */
    MemRequest(uint32_t a, MemOperation m) {
      latency = 0;
      addr = a;
      memOp = m;
    }

    /** Returns the type of memory operation */
    MemOperation getMemOperation() const { return memOp; }

    /** A write miss for WBCache needs to be converted to a read when
     * accessing lower levels to fetch data for the alloated line.
     */
    void mutateWriteToRead() {
      assert(memOp == MemWrite);
      memOp = MemRead;
    }

    /** Returns the latency incurred so far by the memory request */
    uint32_t getLatency() { return latency; }
    /** Adds to the latency incurred by the memory request */
    void addLatency(uint32_t lat) { latency += lat; }

    /** Returns the address for the memory request */
    uint32_t getAddr() const { return addr; }
    /** Sets the address for the memory request */
    void  setAddr(uint32_t a) { addr = a; }
};

#endif   // MEMREQUEST_H
