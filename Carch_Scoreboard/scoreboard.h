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
#define LD_OP 0						// F[DST] = MEM[IMM]
#define ST_OP 1						// MEM[IMM] = F[SRC1]
#define ADD_OP 2					// F[DST] = F[SRC0] + F[SRC1]
#define SUB_OP 3					// F[DST] = F[SRC0] - F[SRC1]
#define MUL_OP 4					// F[DST] = F[SRC0] * F[SRC1]
#define DIV_OP 5					// F[DST] = F[SRC0] / F[SRC1]
#define HALT_OP 6					// exit simulator

// Scoreboard 

// Stages
#define ISSUE 0
#define READ_OP 1
#define EXEC_END 2
#define WRITE_RES 3
#define INST_QUEUE_SIZE 16
#define REG_COUNT 16
#define FU_TYPES 6
#define UNIT_NAME_SIZE 10

// Main memory memory size 4096 words
#define MEM_SIZE (1 << 12)		

// Structure Declarations -------------------------------------------------------

// Functional Unit Constants
typedef struct fu_d {
	int delay_cycles;				// Log FU delay cycles
	int available;					// Log how many FU's of each type are present
}fu_data;

// Functional Unit Status Data Type
typedef struct fu_s {
	int fu_sn;						// FU'S serial number
	bool busy;						// Indicates whether the unit is busy or not
	int f_i;						// Destination register
	int f_j, f_k;					// Source-register numbers
	int q_j, q_k;					// Functional units producing source registers f_j, f_k
	bool r_j, r_k;					// Flags indicating when Fj, Fk are ready
	int remaining_cycles;			// Count how many work cycles have passed
	float result;					// Holds result of functionl unit opperation
}fu_status;

// Instruction Status Data Type
typedef struct inst {
	unsigned int raw_inst;			// 8 hex instruction
	unsigned int opp;				// oppcode
	unsigned int dst;				// destination register
	unsigned int src0;				// source 0 register
	unsigned int src1;				// source 1 register
	int imm;						// immediate value
	unsigned int pc;				// instruction pc value
	int stage_cycle[4];				// Cycle log: cycle issued, cycle read operands, cycle EXEC_ENDute end, cycle write result
	unsigned int issued_fu;			// Number specifying the issued fu index
}inst_status;

// A structure to represent a queue 
typedef struct Queue
{
	int front, rear, size;
	unsigned capacity;
	inst_status *inst_array;
}Buffer;

typedef struct sb {
	fu_status *fu_array[FU_TYPES];					// Pointers to array of FU's status adt		
	Buffer *issued_buffer;
	int reg_res_status[REG_COUNT];					// Register results status array char reg_res_status[REG_COUNT][UNIT_NAME_SIZE];
}scoreboard;

// Global Variables ------------------------------------------------------------
unsigned int mem[MEM_SIZE];			// main memory			
float regs[REG_COUNT];				// Represent X16 32 bit single precision float Registers
Buffer *buffer;						// Instruction queue/buffer
scoreboard sb_a;					// Hold scoreboard during start of clock cycle
scoreboard sb_b;					// Hold scoreboard during end of clock cycle
fu_data fu_const_data[FU_TYPES];	// Hold Fu data read from cfg file
char *fu_names[FU_TYPES];			// Holds the functional unit constant names as strings
char trace_unit[UNIT_NAME_SIZE];	// FU name + id input string for which we produce the trace file
int trace_unit_opp;					// The opperation associated with the tace unit
int trace_unit_index;				// The specific index of the trace unit
bool haltFlag;						// Signal that hald line has been read

// Function Declarations -------------------------------------------------------
void scoreboard_clk(unsigned int cc, bool *exitFlag_ptr, FILE *fp_trace_inst, FILE *fp_truce_unit);
void scoreboard_update();
int scoreboard_init();
void scoreboard_free();
float execOpp(inst_status next_inst);
void update_res_ready(int fu_sn);
void print_unit_trace(FILE *fp_trace_unit, unsigned int cc);
void get_fu_name(int fu_sn,char *str);
bool check_free2write_res(int fu_sn);
void fetch_inst(unsigned int *pc);
bool availableFU(unsigned int opp, unsigned int *free_fu);
void read_mem(FILE *fp_memin, int *lst_line);
int read_config(FILE *fp_config);
void print_memout_regout(FILE *fp_memout, FILE *fp_regout);

// Queue functions
Buffer* createQueue(unsigned capacity);
bool isFull(Buffer* queue);
bool isEmpty(Buffer* queue);
void enqueue(Buffer* queue, inst_status inst);
inst_status dequeue(Buffer* queue);
inst_status front(Buffer* queue);
inst_status rear(Buffer* queue);
void freeQueue(Buffer* queue);

// Aux functions
void parse_inst(inst_status *next_inst);
static inline int bitSel(int x, int msb, int lsb);
static int ArithmetiShiftRight(int n, int numForShift);
static int signExtension(int n);
char *trimwhitespace(char *str);
int parse_line_number(char *str);

#endif // !SCOREBOARD

