/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		- 
*/

#include "scoreboard.h"

int main(int argc, char *argv[])
{
	int last_line,ret=0;
	unsigned int cc,pc;
	bool haltFlag = false;

	// check that we have all cmd line arguments passed - cfg.txt memin.txt memout.txt regout.txt traceinst.txt traceunit.txt
	if (argc != 7) {
		printf("ERROR: Wrong command line usage (cfg.txt memin.txt memout.txt regout.txt traceinst.txt traceunit.txt) \n");
		exit(1);
	}
	// Open files files
	FILE *fp_config = fopen(argv[1], "rt");
	FILE *fp_memin = fopen(argv[2], "rt");
	FILE *fp_memout = fopen(argv[3], "wt");
	FILE *fp_regout = fopen(argv[4], "wt");
	FILE *fp_trace_inst = fopen(argv[5], "wt");
	FILE *fp_truce_unit = fopen(argv[6], "wt");
	// Check if the files opened
	if (!fp_memin || !fp_memout || !fp_trace_inst || !fp_regout || !fp_trace_inst || !fp_config) {
		printf("ERROR: Failed to open files stream \n");
		ret = 1;
		goto cleanup;
	}
	// Read memin.txt into mem and find last non-zero memory entry
	read_mem(fp_memin, &last_line);	
	// Read cfg.txt and updated scoreboard
	if (read_config(fp_config) != 0) {		
		printf("ERROR: Failed read configuration file, check syntax and that all lines are presrnt\n");
		ret = 1;
		goto cleanup;
	}
	// Init scoreboard
	if (scoreboard_init() != 0) {
		printf("ERROR: Failed read configuration file, check syntax and that all lines are presrnt\n");
		ret = 1;
		goto cleanup;
	}
	pc = 0;
	cc = 0;
	while (pc <= last_line && !haltFlag) {
		fetch_inst(&pc);	// Fetch next instruction into instruction queue
		scoreboard_clk(cc, &haltFlag, fp_trace_inst, fp_truce_unit);		// Clock the scoreboard
		scoreboard_update();
		cc++; // incrament clock
	}
	 
	// Write to memout.txt and regout.txt
	print_memouot_regout(fp_memout, fp_regout);
	
cleanup:
	// close files
	fclose(fp_config);
	fclose(fp_memin);
	fclose(fp_memout);
	fclose(fp_regout);
	fclose(fp_trace_inst);
	fclose(fp_truce_unit);
	return ret;
}


/*
// Check for IMM usage in command line
if (rd == 1 || rs == 1 || rt == 1) {
	R[$imm] = signExtension(mem[PC + 1]);
	IMMflag = true;
}
else
	R[$imm] = 0;
// if IMM used then fetch it's value
if (IMMflag) {
	inst = (R[$imm] << 16) + inst;
}
// output to trace file
fprintf(fp_trace, "%08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n", PC, inst, R[0], R[$imm], R[2], R[3], R[4], R[5], R[6], R[7], R[8], R[9], R[10], R[11], R[12], R[13], R[14], R[15]);

// execute command
switch (op)
{

	//add
case(OP_ADD):
	R[rd] = R[rs] + R[rt];
	break;

	//sub
case(OP_SUB):
	R[rd] = R[rs] - R[rt];
	break;

	//and
case(OP_AND):
	R[rd] = R[rs] & R[rt];
	break;

	//or
case(OP_OR):
	R[rd] = R[rs] | R[rt];
	break;

	//sll
case(OP_SLL):
	R[rd] = R[rs] << R[rt];
	break;

	//sra
case (OP_SRA):
	R[rd] = ArithmetiShiftRight(R[rs], R[rt]);
	break;

	//Reserved
case (OP_RSRV):
	break;

	//beq
case(OP_BEQ):
	if (R[rs] == R[rt]) {
		if (IMMflag)
			PC = R[$imm];  //If immidiate value present branch to that address, else branch to R[rd]
		else
			PC = R[rd];
		PCFlag = true; // Jump to address imm, don't increment PC
	}
	break;

	//bgt
case(OP_BGT):
	if (R[rs] > R[rt]) {
		if (IMMflag)
			PC = R[$imm];  //If immidiate value present branch to that address, else branch to R[rd]
		else
			PC = R[rd];
		PCFlag = true; // Jump to address imm, don't increment PC
	}

	break;

	//ble
case(OP_BLE):
	if (R[rs] <= R[rt]) {
		if (IMMflag)
			PC = R[$imm];  //If immidiate value present branch to that address, else branch to R[rd]
		else
			PC = R[rd];
		PCFlag = true; // Jump to address imm, don't increment PC
	}

	break;

	//bne
case(OP_BNE):
	if (R[rs] != R[rt]) {
		if (IMMflag)
			PC = R[$imm];  //If immidiate value present branch to that address, else branch to R[rd]
		else
			PC = R[rd];
		PCFlag = true; // Jump to address imm, don't increment PC
	}
	break;

	//jal - R[15] = next instruction address, pc = R[rd][15:0]
case(OP_JAL):
	if (IMMflag)
		R[$ra] = PC + 2;
	else
		R[$ra] = PC + 1;
	PC = R[$imm];
	PCFlag = true;	// Jump to address imm, don't increment PC
	break;

	//lw	R[rd] = MEM[R[rs]+R[rt]], with sign extension
case(OP_LW):
	R[rd] = signExtension(mem[R[rs] + R[rt]]);
	break;

	//sw
case(OP_SW):
	mem[R[rs] + R[rt]] = bitSel(signExtension(R[rd]), 15, 0);
	break;

	//lhi R[rd][bits 31:16] = R[rs][low bits 15:0]
case(OP_LHI):
	R[rd] = R[rd] & 0x0000ffff;  // Mask the lower two bytes of R[rd]
	R[rd] = R[rd] + (bitSel(R[rs], 15, 0) << 16);   //R[rd][bits 31:16] = R[rs][low bits 15:0]
	break;

	//halt
case(OP_HALT):
	haltFlag = true;
	break;

default:
	break;
}
// Only if PC wasn't dange during this iteration It will increase by one.
if (!PCFlag) {
	PC++;
	if (IMMflag) {
		PC++;
	}
}
else
{
	PCFlag = false;
}
IMMflag = false;
count++;
*/


