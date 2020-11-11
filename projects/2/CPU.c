/** Code by @author Wonsun Ahn
 * 
 * Implements the five stages of the processor pipeline.  The code you will be
 * modifying mainly.
 */

#include <inttypes.h>
#include <assert.h>
#include "CPU.h"
#include "trace.h"
#include "MemObj.h"
#include "MemRequest.h"

unsigned int cycle_number = 0;
unsigned int inst_number = 0;
unsigned int mem_stall_cycles = 0;

std::deque<dynamic_inst> IF, ID, WB;
dynamic_inst EX_ALU = {0}, MEM_ALU = {0};
dynamic_inst EX_lwsw = {0}, MEM_lwsw = {0};

bool is_ALU(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type != ti_NOP && inst.type != ti_LOAD && inst.type != ti_STORE;
}

bool is_lwsw(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type == ti_LOAD || inst.type == ti_STORE;
}

bool is_NOP(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type == ti_NOP;
}

bool is_older(dynamic_inst dinst1, dynamic_inst dinst2) {
  return is_NOP(dinst2) || (!is_NOP(dinst1) && dinst1.seq < dinst2.seq);
}

dynamic_inst get_NOP() {
  dynamic_inst dinst = {0};
  return dinst;
}

void handle_memory_access(dynamic_inst dinst, bool isDataAccess)
{
  if (verbose) {/* print cycles spent for this memory access if verbose=1 */
    if (isDataAccess) {
      printf("[MEM CYCLE: %d] %s\n", cycle_number, get_instruction_string(dinst, ADDR_ONLY));
    } else {
      printf("[IF CYCLE: %d] %s\n", cycle_number, get_instruction_string(dinst, ADDR_ONLY));
    }
  }

  // Create a new memory request and access either data or instruction source
  MemRequest *mreq = NULL;
  if (isDataAccess) {
    if (MEM_lwsw.inst.type == ti_LOAD) {
      mreq = new MemRequest(dinst.inst.Addr, MemRead);
    } else {
      assert(MEM_lwsw.inst.type == ti_STORE);
      mreq = new MemRequest(dinst.inst.Addr, MemWrite);
    }
    config->dataSource->access(mreq);
  } else {
    mreq = new MemRequest(dinst.inst.PC, MemRead);
    config->instSource->access(mreq);
  }
  assert(mreq->getLatency() > 0);

  int stall_cycles;
  if (isDataAccess && dinst.inst.type == ti_STORE) {
    // For store memory accesses, we assume we have an infinitely sized
    // write buffer so that all pending stores go that buffer.  So even when
    // the store request has long latency, it will not cause stalls.
    stall_cycles = 0;
  } else {
    assert(!isDataAccess || dinst.inst.type == ti_LOAD);
    // One cycle delay is already accounted for.  So subtract that.
    stall_cycles = mreq->getLatency() - 1;
  }
  // Delete memory request.  Remember C++ does not have garbage collection!
  delete mreq;

  if (verbose) {/* print cycles spent for this mem instruction if verbose=1 */
    printf("CYCLE: %d -> %d\n", cycle_number, cycle_number + stall_cycles);
    if (debug) {/* print cache contents if debug=1 */
      MemObj::printAllContents();
    }
  }

  cycle_number += stall_cycles;
  mem_stall_cycles += stall_cycles;
}

bool is_finished()
{
  /* Finished when pipeline is completely empty */
  if (IF.size() > 0 || ID.size() > 0) return 0;
  if (!is_NOP(EX_ALU) || !is_NOP(MEM_ALU) || !is_NOP(EX_lwsw) || !is_NOP(MEM_lwsw)) {
    return 0;
  }
  return 1;
}

int writeback()
{
  WB.clear();
  if (is_older(MEM_ALU, MEM_lwsw)) {
    WB.push_back(MEM_ALU);
    MEM_ALU = get_NOP();
    WB.push_back(MEM_lwsw);
    MEM_lwsw = get_NOP();
  }
  else {
    WB.push_back(MEM_lwsw);
    MEM_lwsw = get_NOP();
    WB.push_back(MEM_ALU);
    MEM_ALU = get_NOP();
  }
  return WB.size();
}

int memory()
{
  int insts = 0;
  if (is_NOP(MEM_ALU)) {
    MEM_ALU = EX_ALU;
    EX_ALU = get_NOP();
    insts++;
  }
  if (is_NOP(MEM_lwsw)) {
    MEM_lwsw = EX_lwsw;
    EX_lwsw = get_NOP();
    insts++;
  }

  if (is_lwsw(MEM_lwsw)) {
    /* Insert memory access stalls only on loads. */
    handle_memory_access(MEM_lwsw, true);
  }
  return insts;
}

int issue()
{
  /* in-order issue */
  int insts = 0;
  while (ID.size() > 0) {
    if (is_ALU(ID.front())) {
      if (!is_NOP(EX_ALU)) {
        break;;
      }
      EX_ALU = ID.front();
      ID.pop_front();
    } else if (is_lwsw(ID.front())) {
      if (!is_NOP(EX_lwsw)) {
        break;;
      }
      EX_lwsw = ID.front();
      ID.pop_front();
    } else {
      assert(0);
    }
    insts++;
  }
  return insts;
}

int decode()
{
  int insts = 0;
  while ((int)IF.size() > 0 && (int)ID.size() < config->pipelineWidth) {
    ID.push_back(IF.front());
    IF.pop_front();
    insts++;
  }
  return insts;
}

int fetch()
{
  static unsigned int cur_seq = 1;
  int insts = 0;
  dynamic_inst dinst;
  instruction *tr_entry = NULL;

  /* copy trace entry(s) into IF stage */
  while((int)IF.size() < config->pipelineWidth) {
    size_t size = trace_get_item(&tr_entry); /* put the instruction into a buffer */
    if (size > 0) {
      dinst.inst = *tr_entry;
      dinst.seq = cur_seq++;
      IF.push_back(dinst);
      insts++;
      /* Insert instruction fetch stalls */
      handle_memory_access(dinst, false);
    } else {
      break;
    }
  }
  inst_number += insts;
  return insts;
}

/* Output related functions
 *
 */

void print_pipeline_row(dynamic_inst dinst, const char *stageName)
{
  printf("%45s ", get_instruction_string(dinst, SHORT_FORM));
  printf("%s\n", stageName);
}

void print_pipeline()
{
  printf("=================================================================================\n");
  // Print header
  printf("%45s ", "");
  printf("%s\n", "Pipeline Stage");
  // Print each instruction currently in the pipeline
  for (int i = 0; i < config->pipelineWidth; i++) {
    if(i < (int)WB.size()) {
      print_pipeline_row(WB[i], "WB");
    } else {
      print_pipeline_row(get_NOP(), "WB");
    }
  }
  if (config->pipelineWidth == 2) {
    print_pipeline_row(MEM_ALU, "MEM_ALU");
    print_pipeline_row(MEM_lwsw, "MEM_lwsw");
    print_pipeline_row(EX_ALU, "EX_ALU");
    print_pipeline_row(EX_lwsw, "EX_lwsw");
  } else if (config->pipelineWidth == 1) {
    if (!is_NOP(MEM_ALU)) {
      assert(is_NOP(MEM_lwsw));
      print_pipeline_row(MEM_ALU, "MEM");
    } else {
      print_pipeline_row(MEM_lwsw, "MEM");
    }
    if (!is_NOP(EX_ALU)) {
      assert(is_NOP(EX_lwsw));
      print_pipeline_row(EX_ALU, "EX");
    } else {
      print_pipeline_row(EX_lwsw, "EX");
    }
  } else {
    assert(0);
  }
  for (int i = 0; i < config->pipelineWidth; i++) {
    if(i < (int)ID.size()) {
      print_pipeline_row(ID[i], "ID");
    } else {
      print_pipeline_row(get_NOP(), "ID");
    }
  }
  for (int i = 0; i < config->pipelineWidth; i++) {
    if(i < (int)IF.size()) {
      print_pipeline_row(IF[i], "IF");
    } else {
      print_pipeline_row(get_NOP(), "IF");
    }
  }
  printf("=================================================================================\n");
}

