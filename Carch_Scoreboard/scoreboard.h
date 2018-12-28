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


// Defines ---------------------------------------------------------------------
// opcodes
#define LD 0					// F[DST] = MEM[IMM]
#define ST 1					// MEM[IMM] = F[SRC1]
#define ADD 2					// F[DST] = F[SRC0] + F[SRC1]
#define SUB 3					// F[DST] = F[SRC0] - F[SRC1]
#define MULT 4					// F[DST] = F[SRC0] * F[SRC1]
#define DIV 5					// F[DST] = F[SRC0] / F[SRC1]
#define HALT 6					// exit simulator

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

// Scoreboard stages
#define ISSUE 0
#define READ_OP 1;
#define EXEC 1;
#define WRITE_RES 1;

// Main memory memory size 4096 words
#define MEM_SIZE (1 << 12)		

// Global Variables ------------------------------------------------------------
int mem[MEM_SIZE];					// main memory			
float regs[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};	//Represent X16 32 bit single precision float Registers	
fu_data fu_const_adata[6];			// Hold Fu const data read from cfg file
scoreboard sb_a;					// Hold scoreboard during start of clock cycle
scoreboard sb_b;					// Hold scoreboard during end of clock cycle

// Structure Declarations -------------------------------------------------------
// Instruction Queue Data Type
typedef struct inst{
	int raw_inst;					// 8 hex instruction
	int opp;						// oppcode
	int dst;						// destination register
	int src0;						// source 0 register
	int src1;						// source 1 register
	int imm;						// immediate value
	int pc;							// instruction pc value
	int stage_cycle[4];				// Cycle log: cycle issued, cycle read operands, cycle execute end, cycle write result
}inst_status;

// Functional Unit Constants
typedef struct fu_d{
	int delay_cycles;
	int available;
}fu_data;


// Functional Unit Data Type
typedef struct fu_s{
	int fu_sn;						// FU'S serial number
	int opp;						// Opperation code associated with the fu
	bool busy;						// Indicates whether the unit is busy or not
	int remaining_cycles;			// Count how many work cycles have passed
	int f_i;						// Destination register
	int f_j, f_k;					// Source-register numbers
	int q_j, q_k;					// Functional units producing source registers f_j, f_k
	bool r_j, r_k;					// Flags indicating when Fj, Fk are ready
}fu_status;

typedef struct sb{
	fu_status *fu;					// Pointer to array of FU status adt
	inst_status inst_buffer[16];	// Instruction buffer linked to status adt
	int reg_res_status[16];			// Register results status array 
	int fu_remaining[6];			// Log how many FU's of each type are still available 
}scoreboard;

// Function Declarations -------------------------------------------------------
void read_mem(FILE *fp_memin, int *lst_line);
void read_config(FILE *fp_config);
static inline int bitSel(int x, int msb, int lsb);
static int ArithmetiShiftRight(int n, int numForShift);
static int signExtension(int n);