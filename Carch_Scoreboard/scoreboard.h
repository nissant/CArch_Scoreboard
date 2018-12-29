#pragma once
/*
Authors			- Nisan Tagar (302344031) & Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		-
*/


// Includes --------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h> 

#ifndef SCOREBOARD
#define SCOREBOARD

// Defines ---------------------------------------------------------------------
// opcodes
#define LD_FU 0						// F[DST] = MEM[IMM]
#define ST_FU 1						// MEM[IMM] = F[SRC1]
#define ADD_FU 2					// F[DST] = F[SRC0] + F[SRC1]
#define SUB_FU 3					// F[DST] = F[SRC0] - F[SRC1]
#define MUL_FU 4					// F[DST] = F[SRC0] * F[SRC1]
#define DIV_FU 5					// F[DST] = F[SRC0] / F[SRC1]
#define HALT_FU 6					// exit simulator

// FP registers 
#define F0	0
#define F1	1
#define F2	2
#define F3	3
#define F4	4
#define F5	5
#define F6	6
#define F7	7
#define F8	8
#define F9	9
#define F10	10
#define F11	11
#define F12	12
#define F13	13
#define F14	14
#define F15	15

// Scoreboard 
#define ISSUE 0
#define READ_OP 1;
#define EXEC 2;
#define WRITE_RES 3;
#define INST_QUEUE_SIZE 16
#define REG_COUNT 16
#define FU_TYPES 6
#define UNIT_NAME_SIZE 10

// Main memory memory size 4096 words
#define MEM_SIZE (1 << 12)		

// Structure Declarations -------------------------------------------------------

// A structure to represent a queue 
typedef struct Queue
{
	int front, rear, size;
	unsigned capacity;
	inst_status *inst_array;
}Buffer;


// Instruction Status Data Type
typedef struct inst {
	unsigned int raw_inst;			// 8 hex instruction
	unsigned int opp;				// oppcode
	unsigned int dst;				// destination register
	unsigned int src0;				// source 0 register
	unsigned int src1;				// source 1 register
	int imm;						// immediate value
	unsigned int pc;				// instruction pc value
	unsigned int stage_cycle[4];	// Cycle log: cycle issued, cycle read operands, cycle execute end, cycle write result
}inst_status;


// Functional Unit Constants
typedef struct fu_d {
	int delay_cycles;				// Log FU delay cycles
	int available;					// Log how many FU's of each type are present
}fu_data;


// Functional Unit Status Data Type
typedef struct fu_s {
	int fu_sn;						// FU'S serial number
	bool busy;						// Indicates whether the unit is busy or not
	int remaining_cycles;			// Count how many work cycles have passed
	int f_i;						// Destination register
	int f_j, f_k;					// Source-register numbers
	int q_j, q_k;					// Functional units producing source registers f_j, f_k
	bool r_j, r_k;					// Flags indicating when Fj, Fk are ready
}fu_status;


typedef struct sb {
	fu_status *fu[FU_TYPES];		// Pointers to array of FU's status adt		
	int fu_remaining[FU_TYPES];		// Log remaining fu's
	int reg_res_status[REG_COUNT];	// Register results status array 
}scoreboard;


// Global Variables ------------------------------------------------------------
unsigned int mem[MEM_SIZE];			// main memory			
float regs[REG_COUNT];				// Represent X16 32 bit single precision float Registers

Buffer *buffer;						// Instruction queue/buffer
scoreboard sb_a;					// Hold scoreboard during start of clock cycle
scoreboard sb_b;					// Hold scoreboard during end of clock cycle

fu_data fu_const_data[FU_TYPES];	// Hold Fu data read from cfg file
int fu_count;						// Total number of FU's
char *fu_names[] = {"LD","ST",		// FU names, index corresponds to opcode number
					"ADD","SUB","MUL","DIV"}; 
char trace_unit[UNIT_NAME_SIZE];	// FU name + id input string for which we produce the trace file

// Function Declarations -------------------------------------------------------
int scoreboard_init();
void fetch_inst(unsigned int *pc);
void read_mem(FILE *fp_memin, int *lst_line);
int read_config(FILE *fp_config);
void print_memout_regout(FILE *fp_memout, FILE *fp_regout);

// Queue functions
Buffer* createQueue(unsigned capacity);
int isFull(Buffer* queue);
int isEmpty(Buffer* queue);
void enqueue(Buffer* queue, inst_status inst);
inst_status dequeue(Buffer* queue);
inst_status front(Buffer* queue);
inst_status rear(Buffer* queue);
void freeQueue(Buffer* queue);

// Aux functions
static inline int bitSel(int x, int msb, int lsb);
static int ArithmetiShiftRight(int n, int numForShift);
static int signExtension(int n);
char *trimwhitespace(char *str);
int parse_line_number(char *str);

#endif // !SCOREBOARD

