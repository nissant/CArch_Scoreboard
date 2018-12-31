/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		- 
*/

#include "scoreboard.h"

int main(int argc, char *argv[])
{
	int last_line,ret=0;
	unsigned int cc,pc,instruction;
	bool exitFlag = false;

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
		goto cleanup2;
	}

	pc = 0;
	cc = 0;
	while (!exitFlag) { // pc<= last_line && 
		scoreboard_clk(cc, &exitFlag, fp_trace_inst, fp_truce_unit);		// Clock the scoreboard, this happens in raising clock edge
		fetch_inst(&pc);													// Fetch next instruction into buffer and incrament pc, this happens in raising clock edge
		scoreboard_update();												// Pass end of cycle updates to scoreboard
		// TODO - Print selected unit trace if is currently busy: cycle unit Fi Fj Fk Qj Qk Rj Rk
		// TODO - Check if top instruction has graduated, if true print it's trace and dequeue
		cc++;																// incrament clock cycle
	}
	// Write to memout.txt and regout.txt
	print_memout_regout(fp_memout, fp_regout);
cleanup2:
	scoreboard_free();
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
