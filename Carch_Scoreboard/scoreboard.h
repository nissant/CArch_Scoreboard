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
#define OP_RSRV 6 //Unused Opcode
#define OP_BEQ 7
#define OP_BGT 8
#define OP_BLE 9
#define OP_BNE 10
#define OP_JAL 11
#define OP_LW  12
#define OP_SW  13
#define OP_LHI 14  //R[rd][bits 31:16] = R[rs][low bits 15:0]
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

// main memory memory size
#define MEM_SIZE (1 << 16)

// Global Variables ------------------------------------------------------------

// Function Declarations -------------------------------------------------------


// extract multiple bits
static inline int bitSel(int x, int msb, int lsb)
{
	if (msb == 31 && lsb == 0)
		return x;
	return (x >> lsb) & ((1 << (msb - lsb + 1)) - 1);
}

//arithmetic shift right (sra command)
static int ArithmetiShiftRight(int n, int numForShift) {
	if ((n & 0x80000000) == 0x80000000) {
		for (int i = 0; i < numForShift; i++)
		{
			n = n >> 1;
			n = n | 0x80000000;
		}
	}
	else
	{
		n = n >> numForShift;
	}
	return n;
}

//sign extend 16 bit to 32 bit
static int signExtension(int n) {
	int value = (0x0000FFFF & n);
	int mask = 0x00008000;
	if (mask & n) {
		value += 0xFFFF0000;
	}
	return value;
}