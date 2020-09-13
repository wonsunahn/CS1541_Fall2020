#ifndef CPU_H
#define CPU_H

#include <deque>
#include "config.h"

enum opcode {
	ti_NOP = 0,
	ti_RTYPE,
	ti_ITYPE,
	ti_LOAD,
	ti_STORE,
	ti_BRANCH,
	ti_JTYPE,
	ti_SPECIAL,
	ti_JRTYPE
};

typedef struct {
	unsigned char type;		// see above
	unsigned char sReg_a;		// 1st operand
	unsigned char sReg_b;		// 2nd operand
	unsigned char dReg;		// dest. operand
	unsigned int PC;		// program counter
	unsigned int Addr;		// mem. address
} instruction;

typedef struct {
	instruction inst;		// static instruction fetched to create this dynamic instruction
	unsigned int seq;		// dynamic sequence number (important for in-order commit)
} dynamic_inst;

extern unsigned int cycle_number;
extern unsigned int inst_number;

extern std::deque<dynamic_inst> IF, ID, WB;
extern dynamic_inst EX_ALU, MEM_ALU;
extern dynamic_inst EX_lwsw, MEM_lwsw;

bool is_finished();
bool is_NOP(dynamic_inst dinst);
dynamic_inst get_NOP();

int writeback();
int memory();
int issue();
int decode();
int fetch();

#endif /* #define CPU_H */
