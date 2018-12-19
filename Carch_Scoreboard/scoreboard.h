#pragma once
/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
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
#define OP_ADD 0
#define OP_SUB 1
#define OP_AND 2
#define OP_OR  3
#define OP_SLL 4
#define OP_SRA 5
#define OP_RSRV 6				//Unused Opcode
#define OP_BEQ 7
#define OP_BGT 8
#define OP_BLE 9
#define OP_BNE 10
#define OP_JAL 11
#define OP_LW  12
#define OP_SW  13
#define OP_LHI 14				//R[rd][bits 31:16] = R[rs][low bits 15:0]
#define OP_HALT 15

// registers 
#define $zero 0
#define $imm 1
#define $v0 2
#define $a0  3
#define $a1 4
#define $t0 5
#define $t1 6 
#define $t2 7
#define $t3 8
#define $s0 9
#define $s1 10
#define $s2 11
#define $gp 12
#define $sp 13
#define $fp 14  
#define $ra 15


#define MEM_SIZE (1 << 12)		// main memory memory size 4096 words

// Global Variables ------------------------------------------------------------
	
int mem[MEM_SIZE];				// main memory
float R[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};		//Represent X16 32 bit single precision float Registers

typedef struct instruction{
	long int raw_inst;			// 8 hex instruction
	int opp;					// oppcode
	int dst;					// destination register
	int src0;					// source 0 register
	int src1;					// source 1 register
	int imm;					// immediate value
	int pc;						// instruction pc value
	int unit;					// number of handling functional unit. The type of the FU is already given by the opp field.
	int stage_cycle[4];			// Cycle log: cycle issued, cycle read operands, cycle execute end, cycle write result
}inst_data;

typedef struct scoreboard{
	


}sb;


// Function Declarations -------------------------------------------------------
static inline int bitSel(int x, int msb, int lsb);
static int ArithmetiShiftRight(int n, int numForShift);
static int signExtension(int n);