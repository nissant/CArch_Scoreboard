/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		-
*/

#include "scoreboard.h"

// Function Definitions -------------------------------------------------------

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


void read_mem(FILE *fp_memin, int *lst_line) {
	// convert each line in memin from HEX to DEC and add it to mem[i]
	int i = 0;
	memset(mem, 0, MEM_SIZE * sizeof(int));		// zero memory
	while (!feof(fp_memin)) {
		if (fscanf(fp_memin, "%08X\n", &mem[i]) != 1)
			break;
		i++;
	}

	int last_line = MEM_SIZE - 1;
	while (last_line >= 0 && mem[last_line] == 0)
		last_line--;
	*lst_line = last_line;
}

void read_config(FILE *fp_config) {
	//int i = 0;
	char *tmp_str;
	char sub_str[] = "sub_nr_units =";
	char mul_str[] = "mul_nr_units =";
	char div_str[] = "div_nr_units =";
	char ld_str[] = "ld_nr_units =";
	char st_str[] = "st _nr_units =";

	memset(mem, 0, MEM_SIZE * sizeof(int));		// zero memory
	while (!feof(fp_config)) {
		if (fscanf(fp_config, "%08X\n", tmp_str) != 1)
			break;

		if (strstr(tmp_str, "add_nr_units =") != NULL) {
			 
		//i++;

		}
	}
}

int paese_line(char *str) {

	char *quotPtr = strchr(attrPtr, qolon);
	if (quotPtr == NULL)
	{
		... // Handle error
	}

}

scoreboard_clk() {
	// 1) Issue one instruction 
	op = bitSel(inst, 15, 12);
	rd = bitSel(inst, 11, 8);
	rs = bitSel(inst, 7, 4);
	rt = bitSel(inst, 3, 0);
	//imm = 
// 2) Read operands
// 3) Execution
// 4) Write results
}

