/** Code by @author Wonsun Ahn
 * 
 * Implements the five stages of the processor pipeline.  The code you will be
 * modifying mainly.
 */

#include <inttypes.h>
#include <assert.h>
#include "CPU.h"
#include "trace.h"

//used to control raw bubble insertions
int rawFound = 0;
int useLoad = 0;
int optUseLoad1 = 0;
int optUseLoad2 = 0;

unsigned int cycle_number = 0;
unsigned int inst_number = 0;

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

//fuction to determine if instruction is a store type
bool is_store(dynamic_inst dinst) {
        instruction inst = dinst.inst;
        return  inst.type == ti_STORE;
}
//fuction to determine if instruction is a load type
bool is_load(dynamic_inst dinst) {
        instruction inst = dinst.inst;
        return  inst.type == ti_LOAD;
}

//fuction to determine if instruction is a R type
bool is_rType(dynamic_inst dinst) {
        instruction inst = dinst.inst;
        return  inst.type == ti_RTYPE;
}

//fuction to determine if instruction is a branch
bool is_branch(dynamic_inst dinst) {
        instruction inst = dinst.inst;
        return  inst.type == ti_BRANCH;
}

//fuction to determine if instruction is an I type
bool is_iType(dynamic_inst dinst) {
        instruction inst = dinst.inst;
        return  inst.type == ti_ITYPE;
}

//function to determine if an instruction reads
bool is_reader(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  return inst.type == ti_RTYPE || inst.type == ti_ITYPE || inst.type == ti_STORE || inst.type == ti_BRANCH || inst.type == ti_JRTYPE;
}

//function to determine if an anstruction writes
bool is_writer(dynamic_inst dinst) {
  instruction inst = dinst.inst;
  
  return inst.type == ti_RTYPE ||  inst.type == ti_ITYPE ||  inst.type == ti_LOAD;
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
  static unsigned int cur_seq = 1;
  WB.clear();
  
  if (is_older(MEM_ALU, MEM_lwsw)) {
                //takes care of regfile strucural Hazard and WAW part 1 (optimized)
                if(config->regFileWritePorts == 1 && is_rType(MEM_ALU) && !is_store(MEM_lwsw)){
                        WB.push_back(MEM_ALU);
                        MEM_ALU = get_NOP();
                        WB.push_back(get_NOP());
                }
                else{
                        WB.push_back(MEM_ALU);
                        MEM_ALU = get_NOP();
                        if(config->regFileWritePorts == 2 && is_writer(WB.front()) && is_writer(MEM_lwsw)&& WB.front().inst.dReg == MEM_lwsw.inst.dReg){
                                WB.push_back(get_NOP());
                        }
                        else{
                                WB.push_back(MEM_lwsw);
                                MEM_lwsw = get_NOP();
                        }
                }
        }
        
  else {
                //takes care of regfile strucural Hazard and WAW part 2 (optimized)
                if(config->regFileWritePorts == 1 && is_rType(MEM_ALU) && !is_store(MEM_lwsw)){
                        WB.push_back(MEM_lwsw);
                        MEM_lwsw = get_NOP();
                        WB.push_back(get_NOP());
                }
                else{
                        WB.push_back(MEM_lwsw);
                        MEM_lwsw = get_NOP();
                        if(config->regFileWritePorts == 2 && is_writer(WB.front()) && is_writer(MEM_lwsw) && WB.front().inst.dReg == MEM_lwsw.inst.dReg){
                                WB.push_back(get_NOP());
                        }
                        else{
                                WB.push_back(MEM_ALU);
                                MEM_ALU = get_NOP();
                        }
                }
        }
  //end regfile structural hazard and WAW detection

  if (verbose) {/* print the instruction exiting the pipeline if verbose=1 */
    for (int i = 0; i < (int) WB.size(); i++) {
      printf("[%d: WB] %s\n", cycle_number, get_instruction_string(WB[i], true));
      if(!is_NOP(WB[i])) {
        if(config->pipelineWidth > 1 && config->regFileWritePorts == 1) {
          // There is a corner case where an instruction without a
          // destination register can get pulled in out of sequence but
          // other than that, it should be strictly in-order.
        } else {
          assert(WB[i].seq == cur_seq);
        }
        cur_seq++;
      }
    }
  }
  return WB.size();
}


int memory()
{       
  //code that enables optimized load after use for single width cpu
  if(config->pipelineWidth == 1 && is_load(EX_lwsw) && !ID.empty() && is_rType(ID.front())){
        optUseLoad1 = 1;
  }
  
  //code that enables optimized load after use for double width cpu
  if(config->pipelineWidth == 2 && is_load(ID.front()) && is_rType(ID.back())){
        optUseLoad2 = 1;
  }
        
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
  return insts;
}


int issue()
{
  /* in-order issue */
  int insts = 0;
  while (ID.size() > 0) {
          
         if((is_load(MEM_lwsw) && (is_branch(ID.front()))))
        {
                if(config->pipelineWidth==1)
                {
                        break;
                }
        }
        //code to stop a use after load hazard optimized for single width
        if(config->enableForwarding && config->pipelineWidth == 1 && optUseLoad1 == 1 && (EX_lwsw.inst.dReg == IF.front().inst.sReg_a || EX_lwsw.inst.dReg == IF.front().inst.sReg_b)){
                optUseLoad1 = 0;
                break;
        }
        
        //code to stop a use after load hazard optimized for double width
        if(config->enableForwarding && config->pipelineWidth == 2 && optUseLoad2 == 1 && (ID.front().inst.dReg == ID.back().inst.sReg_a || ID.front().inst.dReg == ID.back().inst.sReg_b)){
            EX_lwsw = ID.front();
                ID.pop_front();
                optUseLoad2++;
                break;
        }
        //code to stop a use after load hazard optimized for double width (part 2)
        if(optUseLoad2 == 2){
                optUseLoad2 = 0;
                break;
        }
          
        //enables code to stop a use after load hazard 
        if(!IF.empty() && !ID.empty() && is_rType(IF.front()) && is_load(ID.back()) && (ID.back().inst.dReg == IF.front().inst.sReg_a || ID.back().inst.dReg == IF.front().inst.sReg_b)){
                useLoad = 1;
        } 
        
        
    if (is_ALU(ID.front())) {
      if (!is_NOP(EX_ALU)) {
        break;
      }
      EX_ALU = ID.front();
      ID.pop_front();
    } else if (is_lwsw(ID.front())) {
      if (!is_NOP(EX_lwsw)) {
        break;
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
  while ((int)IF.size() > 0 && (int)ID.size() < config->pipelineWidth){
                
                if((is_rType(EX_ALU)||is_rType(MEM_ALU)||is_rType(WB.front()))&&is_rType(IF.front())&&!config->enableForwarding)
                {
                        break;
                }
                
                //code to stop a use after load hazard
                if(!config->enableForwarding && useLoad == 1){
                        useLoad++;
                        break;
                }
                if(!config->enableForwarding && useLoad == 2){
                        useLoad++;
                        break;
                }
                if(!config->enableForwarding && useLoad == 3){
                        useLoad = 0;
                        break;
                }
                else{
                        //RAW data hazard taken care of
                        if(!config->enableForwarding && (!IF.empty()) && (!ID.empty()) && is_reader(IF.front()) && is_writer(ID.front()) && ((ID.front().inst.dReg == IF.front().inst.sReg_a) || (ID.front().inst.dReg == IF.front().inst.sReg_b))){
                                break;
                        }               
                        if(!config->enableForwarding && is_reader(IF.front()) && is_writer(EX_lwsw) && (EX_lwsw.inst.dReg == IF.front().inst.sReg_a || EX_lwsw.inst.dReg == IF.front().inst.sReg_b)){
                                break;
                        }       
                        if(is_reader(IF.front()) && is_writer(MEM_lwsw) && (MEM_lwsw.inst.dReg == IF.front().inst.sReg_a || MEM_lwsw.inst.dReg == IF.front().inst.sReg_b)){
                                rawFound = 1;
                                break;
                        }
                        if(rawFound==1){
                                if(is_reader(IF.front()) && is_writer(WB.front()) && (WB.front().inst.dReg == IF.front().inst.sReg_a || WB.front().inst.dReg == IF.front().inst.sReg_b)){
                                        break;
                                }
                                else{
                                        ID.push_front(IF.front());
                                        IF.pop_front();
                                        insts++;
                                        break;
                                }
                        }
                }
                //end RAW detection
                ID.push_back(IF.front());
                IF.pop_front();
                insts++;
                rawFound=0;
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
                
                
                if(is_load(MEM_lwsw) && config->pipelineWidth==2&&!config->enableForwarding)
                        break;
                
                //control hazard taken care of (optimized)
                if(config->branchTargetBuffer==false){
                        if((!IF.empty() )&& is_branch(IF.front())){
                                if(IF.front().inst.Addr != IF.front().inst.PC+4){
                                        break;
                                }
                        }
                        if((!ID.empty() ) && is_branch(ID.front())){
                                if(ID.front().inst.Addr != ID.front().inst.PC+4){
                                        break;
                                }
                        }
                        if(is_branch(EX_ALU)){
                                if(EX_ALU.inst.Addr != EX_ALU.inst.PC+4){
                                        break;
                                }
                        }
                }
                else if(config->pipelineWidth == 2){
                        if(is_branch(IF.front())){
                                if(IF.front().inst.Addr != IF.front().inst.PC+4){
                                        break;
                                }
                        }
                }
                //end control hazard detection
          
                //structural memory hazard taken care of and optimized
                if(!config->splitCaches && config->pipelineWidth == 1){
                        if(is_load(MEM_lwsw))
                        {
                                if(IF.empty())
                                {
                                        break;
                                }
                        }
                }
                else if(!config->splitCaches && config->pipelineWidth == 2)
                {
                        if(is_load(MEM_lwsw))
                        {
                                if(is_load(IF.front()))
                                {
                                        IF.push_back(get_NOP());
                                }
                        }
                }
                //end structural memory hazard detection
                
                size_t size = trace_get_item(&tr_entry); /* put the instruction into a buffer */
                if (size > 0) {
                        dinst.inst = *tr_entry;
                        dinst.seq = cur_seq++;
                        IF.push_back(dinst);
                        insts++;
                } else {
                        break;
                }
        
                if (verbose) {/* print the instruction entering the pipeline if verbose=1 */
                        printf("[%d: IF] %s\n", cycle_number, get_instruction_string(IF.back(), true));
                }
        }
  inst_number += insts;
  return insts;
}

