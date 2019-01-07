/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		-
*/

#include "scoreboard.h"
float regs[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };					//Represent X16 32 bit single precision float Registers	
char *fu_names[FU_TYPES] = { "LD","ST","ADD","SUB","MUL","DIV" };		// FU names, index corresponds to opcode number
bool haltFlag = false;														// Signal that hald line has been read
				   

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

int read_config(FILE *fp_config) {
	int temp_num,i,cfg_count=0;
	char tmp_str[20];
	char *ptr;

	while (!feof(fp_config)) {
		if (fscanf(fp_config, "%[^\n]\n", tmp_str) != 1)
			break;
		// Look for #fu lines
		if (strstr(tmp_str, "add_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ADD_OP].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "sub_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[SUB_OP].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "mul_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[MUL_OP].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "div_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[DIV_OP].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "ld_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[LD_OP].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "st_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ST_OP].available = temp_num;
			cfg_count++;
		}
		// Look for fu delay lines
		else if (strstr(tmp_str, "add_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ADD_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "sub_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[SUB_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "mul_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[MUL_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "div_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[DIV_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "ld_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[LD_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "st_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ST_OP].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "trace_unit") != NULL) {
			char *quotPtr = strchr(tmp_str, '=');
			if (quotPtr == NULL)
			{
				return -1;
			}
			quotPtr++;
			quotPtr = trimwhitespace(quotPtr);
			strcat(trace_unit, quotPtr);
			cfg_count++;
		}

	}
	// Parse trace_unit string
	for (i = 0; i < FU_TYPES; i++) {
		ptr = strstr(trace_unit, fu_names[i]);
		if (ptr != NULL) {
			trace_unit_opp = i;
			ptr += strlen(fu_names[i]);
			ptr = trimwhitespace(ptr);
			trace_unit_index = (int)strtol(ptr, NULL, 10);
		}
	}

	if (cfg_count == 13) {
		//All expected lines where found and read
		return 0;
	}
	else {
		return -1;
	}
}

int parse_line_number(char *str) {

	char *quotPtr = strchr(str, '=');
	if (quotPtr == NULL)
	{
		return -1;
	}
	quotPtr++;
	quotPtr = trimwhitespace(quotPtr);
	return (strtol(quotPtr, NULL, 10));
}

/*
Function: trimwhitespace
------------------------
Description – The function receive pointer to a string and trimms the string from white spaces
Parameters	– *str is a pointer to a string to be trimmed.
Returns		– Retrun pointer to trimmed string
*/
char *trimwhitespace(char *str)
{
	char *end;

	// Trim leading space
	while (isspace((unsigned char)*str)) str++;

	if (*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}

void fetch_inst(unsigned int *pc) {
	if (isFull(buffer)) {
		return;
	}
	else {
		inst_status new_inst;
		new_inst.raw_inst = mem[*pc];
		new_inst.pc = *pc;
		new_inst.stage_cycle[ISSUE] = -1;
		new_inst.stage_cycle[READ_OP] = -1;
		new_inst.stage_cycle[EXEC_END] = -1;
		new_inst.stage_cycle[WRITE_RES] = -1;
		enqueue(buffer, new_inst);
		*pc = *pc + 1;
	}
	return;
}

void scoreboard_clk(unsigned int cc, bool *exitFlag_ptr, FILE *fp_trace_inst, FILE *fp_trace_unit) {
	// TODO - Concider implememnting the issued instructions list in a dynmically allocated array instead of queue....
	inst_status next_inst;
	unsigned int free_fu;
	char tmp[UNIT_NAME_SIZE];
	char snum[3];
	int i, n;

	// Issue Phase - Try to issue one instruction as long as halt flag is not raised and instruction present in buffer
	if (isEmpty(buffer) == false && haltFlag==false) {	// Check if possible to issue instruction or if halt opperation
		next_inst = front(buffer);
		parse_inst(&next_inst);
		if (next_inst.opp == HALT_OP) {
			haltFlag = true; // TODO - Raise flag only when all remaining instructions graduated
		}
		if (availableFU(next_inst.opp, &free_fu) && (sb_a.reg_res_status[next_inst.dst] == -1 || next_inst.opp == ST_OP)) {
			// Issue procedure...
			sb_b.fu_array[next_inst.opp][free_fu].busy = true;
			next_inst.stage_cycle[ISSUE] = cc;
			next_inst.issued_fu = free_fu;
			if (next_inst.opp != ST_OP)
				sb_b.reg_res_status[next_inst.dst] = sb_a.fu_array[next_inst.opp][free_fu].fu_sn;

			sb_b.fu_array[next_inst.opp][free_fu].f_i = next_inst.dst;
			sb_b.fu_array[next_inst.opp][free_fu].f_j = next_inst.src0;
			sb_b.fu_array[next_inst.opp][free_fu].f_k = next_inst.src1;

			if (next_inst.opp == LD_OP) {		// F[DST] = MEM[IMM]
				sb_b.fu_array[next_inst.opp][free_fu].q_j = -1;
				sb_b.fu_array[next_inst.opp][free_fu].q_k = -1;
				sb_b.fu_array[next_inst.opp][free_fu].r_j = true;
				sb_b.fu_array[next_inst.opp][free_fu].r_k = true;
			}
			else if (next_inst.opp == ST_OP) {	// MEM[IMM] = F[SRC1]
				sb_b.fu_array[next_inst.opp][free_fu].q_j = -1;
				sb_b.fu_array[next_inst.opp][free_fu].q_k = sb_a.reg_res_status[next_inst.src1];
				sb_b.fu_array[next_inst.opp][free_fu].r_j = true;
				if (sb_a.reg_res_status[next_inst.src1] == -1)		// If no fu is assigned for writing to k source, it is assigned for read opperands stage
					sb_b.fu_array[next_inst.opp][free_fu].r_k = true;
				else
					sb_b.fu_array[next_inst.opp][free_fu].r_k = false;
			}
			else {
				sb_b.fu_array[next_inst.opp][free_fu].q_j = sb_a.reg_res_status[next_inst.src0];
				sb_b.fu_array[next_inst.opp][free_fu].q_k = sb_a.reg_res_status[next_inst.src1];
				if (sb_a.reg_res_status[next_inst.src0] == -1)		// If no fu is assigned for writing to j source, it is assigned for read opperands stage
					sb_b.fu_array[next_inst.opp][free_fu].r_j = true;
				else
					sb_b.fu_array[next_inst.opp][free_fu].r_j = false;

				if (sb_a.reg_res_status[next_inst.src1] == -1)		// If no fu is assigned for writing to k source, it is assigned for read opperands stage
					sb_b.fu_array[next_inst.opp][free_fu].r_k = true;
				else
					sb_b.fu_array[next_inst.opp][free_fu].r_k = false;
			}
			enqueue(sb_b.issued_buffer,next_inst);
			dequeue(buffer);
		}
	}

	// Check if any instructions currently issued
	if (isEmpty(sb_a.issued_buffer)) {
		if (haltFlag)
			*exitFlag_ptr = true;	// Issue queue is empty and halt flag has been raised
		return;
	}

	// Print to unit trace file if the unit is busy at this clock cycle
	if (sb_a.fu_array[trace_unit_opp][trace_unit_index].busy) {
		print_unit_trace(fp_trace_unit, cc);
	}

	// At this point, issued instruction are present in the scoreboard queue (begining of clock cycle - sb_a)
	// Itterate over issued queue and advance: Read Operand, Exec, Exec_End and Write results phases
	for (n = 0; n < sb_a.issued_buffer->size; n++) {
		i = (sb_a.issued_buffer->front + n) % sb_a.issued_buffer->capacity;
		next_inst = sb_a.issued_buffer->inst_array[i];
		
		if (next_inst.stage_cycle[READ_OP] == -1) {			// Instruction is waiting for READ_OP & Execution start stage
			if (next_inst.opp == LD_OP || next_inst.opp == ST_OP) { // Check that target memeoy cell is not dirty before allowing load/store opp
				if (isMemDirty(next_inst))
					continue;
			}
			if (sb_a.fu_array[next_inst.opp][next_inst.issued_fu].r_j &&sb_a.fu_array[next_inst.opp][next_inst.issued_fu].r_k) { // Check operands ready for reading
				sb_b.issued_buffer->inst_array[i].stage_cycle[READ_OP] = cc;						// log cycle
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].r_j = false;						// log s.b
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].r_k = false;						// log s.b
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].result = execOpp(next_inst);		// get fu results
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].remaining_cycles--;				// decrement cycle counter
			}
		}
		else if (next_inst.stage_cycle[EXEC_END] == -1) {	// Instruction is waiting to finish execution stage
			sb_b.fu_array[next_inst.opp][next_inst.issued_fu].remaining_cycles--;
			if (sb_b.fu_array[next_inst.opp][next_inst.issued_fu].remaining_cycles <= 0) {
				sb_b.issued_buffer->inst_array[i].stage_cycle[EXEC_END] = cc;						// log cycle
			}
		}
		else if (next_inst.stage_cycle[WRITE_RES] == -1) {	// Instruction is waiting to write results and graduate
			// Check conditions and write results
			if (check_free2write_res(sb_a.fu_array[next_inst.opp][next_inst.issued_fu].fu_sn)) {
				// Write results phase
				if (next_inst.opp == ST_OP) {
					memcpy(&mem[next_inst.imm], &sb_a.fu_array[next_inst.opp][next_inst.issued_fu].result,sizeof(float));
				}
				else {
					regs[next_inst.dst] = sb_a.fu_array[next_inst.opp][next_inst.issued_fu].result;
				}
				// Update s.b clean up fu and reset fields
				sb_b.issued_buffer->inst_array[i].stage_cycle[WRITE_RES] = cc;
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].busy = false;
				sb_b.fu_array[next_inst.opp][next_inst.issued_fu].remaining_cycles = fu_const_data[next_inst.opp].delay_cycles;
				sb_b.reg_res_status[next_inst.dst] = -1;
				update_res_ready(sb_a.fu_array[next_inst.opp][next_inst.issued_fu].fu_sn);
			}
		}
	}

	// Check if top instruction queue has graduated 
	next_inst = front(sb_b.issued_buffer);
	if (next_inst.stage_cycle[WRITE_RES] > -1) {
		//instruction pc unit cycle issued cycle read operands cycle execute end cycle write result
		strcpy(tmp, fu_names[next_inst.opp]);
		itoa(next_inst.issued_fu, snum, 10);
		strcat(tmp, snum);
		fprintf(fp_trace_inst, "%08x %d %s %d %d %d %d\n", next_inst.raw_inst, next_inst.pc, tmp, next_inst.stage_cycle[ISSUE],next_inst.stage_cycle[READ_OP], next_inst.stage_cycle[EXEC_END], next_inst.stage_cycle[WRITE_RES]);
		dequeue(sb_b.issued_buffer);
	}
}

void print_unit_trace(FILE *fp_trace_unit, unsigned int cc) {
	
	char Rj[5],Rk[5];
	char *Qj[UNIT_NAME_SIZE], *Qk[UNIT_NAME_SIZE];
	if (sb_a.fu_array[trace_unit_opp][trace_unit_index].r_j) {
		strcpy(Rj, "Yes\0");
	}
	else {
		strcpy(Rj, "No\0");
	}
	if (sb_a.fu_array[trace_unit_opp][trace_unit_index].r_k) {
		strcpy(Rk, "Yes\0");
	}
	else {
		strcpy(Rk, "No\0");
	}
	if (sb_a.fu_array[trace_unit_opp][trace_unit_index].q_j == -1) {
		strcpy(Qj, "-");
	}
	else {
		get_fu_name(sb_a.fu_array[trace_unit_opp][trace_unit_index].q_j, Qj);
	}

	if (sb_a.fu_array[trace_unit_opp][trace_unit_index].q_k == -1) {
		strcpy(Qk, "-");
	}
	else {
		get_fu_name(sb_a.fu_array[trace_unit_opp][trace_unit_index].q_k, Qk);
	}
	//cycle unit Fi Fj Fk Qj Qk Rj Rk
	fprintf(fp_trace_unit, "%d %s F%d F%d F%d %s %s %s %s\n", cc, trace_unit, sb_a.fu_array[trace_unit_opp][trace_unit_index].f_i, sb_a.fu_array[trace_unit_opp][trace_unit_index].f_j, sb_a.fu_array[trace_unit_opp][trace_unit_index].f_k, Qj, Qk, Rj, Rk);
}

void get_fu_name(int fu_sn, char *str) {
	int i, j;
	char snum[3];
	// Itterate over FU's 
	for (i = 0; i < FU_TYPES; i++) {
		for (j = 0; j < fu_const_data[i].available; j++) {
			if (sb_a.fu_array[i][j].fu_sn == fu_sn) {
				strcpy(str, fu_names[i]);
				itoa(j, snum, 10);
				strcat(str, snum);
			}
		}
	}
}

void update_res_ready(int fu_sn) {
	int i, j;
	// Itterate over FU's 
	for (i = 0; i < FU_TYPES; i++) {
		for (j = 0; j < fu_const_data[i].available; j++) {
			if (sb_a.fu_array[i][j].q_j == fu_sn || sb_b.fu_array[i][j].q_j == fu_sn) {
				sb_b.fu_array[i][j].r_j = true;		// Update r_j
				//sb_a.fu_array[i][j].r_j = true;		// Update r_j
				sb_b.fu_array[i][j].q_j = -1;		// Delete q_j
				//sb_a.fu_array[i][j].q_j = -1;		// Delete q_j
			}
			if (sb_a.fu_array[i][j].q_k == fu_sn || sb_b.fu_array[i][j].q_k == fu_sn) {
				sb_b.fu_array[i][j].r_k = true;		// Update r_k
				//sb_a.fu_array[i][j].r_k = true;		// Update r_k
				sb_b.fu_array[i][j].q_k = -1;		// Delete q_k
				//sb_a.fu_array[i][j].q_k = -1;		// Delete q_k
			}
		}
	}
}

bool check_free2write_res(int fu_sn) {
	int i, j;
	// Itterate over FU's 
	for (i = 0; i < FU_TYPES; i++) {
		for (j = 0; j < fu_const_data[i].available; j++) {
			if (sb_a.fu_array[i][j].q_j == fu_sn && sb_a.fu_array[i][j].r_j == true) {
				return false;
			}
			if (sb_a.fu_array[i][j].q_k == fu_sn && sb_a.fu_array[i][j].r_k == true) {
				return false;
			}
		}
	}
	return true;
}

bool isMemDirty(inst_status inst) {
	int n, i;
	inst_status next_inst;

	// Itterate over issued instruction queue
	for (n = 0; n < sb_b.issued_buffer->size; n++) {
		i = (sb_b.issued_buffer->front + n) % sb_a.issued_buffer->capacity;
		next_inst = sb_b.issued_buffer->inst_array[i];
		if (next_inst.opp == ST_OP && (next_inst.stage_cycle[READ_OP] != -1 && next_inst.stage_cycle[WRITE_RES] == -1) && next_inst.imm == inst.imm) {	//&& next_inst.stage_cycle[READ_OP] != -1
			// A more mature store instruction is already writing to the same cell. MEM[IMM] = F[SRC1]
			return true;
		}
	}
	return false;
}


void parse_inst(inst_status *next_inst) {
	unsigned int temp;
	temp = next_inst->raw_inst;
	next_inst->opp = bitSel(temp,27,24);
	next_inst->dst = bitSel(temp, 23, 20);
	next_inst->src0 = bitSel(temp, 19, 16);
	next_inst->src1 = bitSel(temp, 15, 12);
	next_inst->imm = bitSel(temp, 11, 0);
}

float execOpp(inst_status next_inst) {
	float x;
	switch (next_inst.opp) {
	case (LD_OP):	// F[DST] = MEM[IMM]
		memcpy(&x, &mem[next_inst.imm], sizeof (float));
		return x;
		break;
	case (ST_OP):	// MEM[IMM] = F[SRC1]
		return (regs[next_inst.src1]);
		break;
	case (ADD_OP):	// F[DST] = F[SRC0] + F[SRC1]
		return (regs[next_inst.src0]+regs[next_inst.src1]);
		break;
	case (SUB_OP):	// F[DST] = F[SRC0] - F[SRC1]
		return (regs[next_inst.src0] - regs[next_inst.src1]);
		break;
	case (MUL_OP):	// F[DST] = F[SRC0] * F[SRC1]
		return (regs[next_inst.src0] * regs[next_inst.src1]);
		break;
	case (DIV_OP):	// F[DST] = F[SRC0] / F[SRC1]
		return (regs[next_inst.src0] / regs[next_inst.src1]);
		break;
	}
	return;
}

bool availableFU(unsigned int opp,unsigned int *free_fu) {
	int i;
	for (i = 0; i < fu_const_data[opp].available; i++) {
		if (sb_a.fu_array[opp][i].busy == false) {
			*free_fu = i;
			return true;
		}
	}
	return false;
}

void scoreboard_update() {
	int i,j;
	// Update FU status
	for (i = 0; i < FU_TYPES; i++) {
		for(j=0;j<fu_const_data[i].available;j++)
		//sb_a.fu_array[i] = sb_b.fu_array[i];
		memcpy(&sb_a.fu_array[i][j], &sb_b.fu_array[i][j], sizeof(fu_status));
	}

	// Update instruction status in issue queue
	sb_a.issued_buffer->capacity = sb_b.issued_buffer->capacity;
	sb_a.issued_buffer->front = sb_b.issued_buffer->front;
	sb_a.issued_buffer->rear = sb_b.issued_buffer->rear;
	sb_a.issued_buffer->size = sb_b.issued_buffer->size;
	for (i = 0; i < sb_b.issued_buffer->capacity; i++) {
		memcpy(&sb_a.issued_buffer->inst_array[i], &sb_b.issued_buffer->inst_array[i], sizeof(inst_status));
		//sb_a.issued_buffer->inst_array[i] = sb_b.issued_buffer->inst_array[i];
	}
	// Update register results status
	for (i = 0; i < REG_COUNT; i++) {
		sb_a.reg_res_status[i] = sb_b.reg_res_status[i];
	}
}

int scoreboard_init() {
	int i, j;
	int fu_count = 0,fu_sn=0;
	
	// Allocate FU's 
	for (i = 0; i < FU_TYPES; i++) {
		sb_a.fu_array[i] = (fu_status*)malloc(fu_const_data[i].available * sizeof(fu_status));
		sb_b.fu_array[i] = (fu_status*)malloc(fu_const_data[i].available * sizeof(fu_status));
		if (sb_a.fu_array[i] == NULL || sb_b.fu_array[i] == NULL) {
			return -1;
		}
		fu_count += fu_const_data[i].available;
		// Freee all fu's of type i
		for (j = 0; j < fu_const_data[i].available; j++) {
			sb_a.fu_array[i][j].remaining_cycles = fu_const_data[i].delay_cycles;
			sb_b.fu_array[i][j].remaining_cycles = fu_const_data[i].delay_cycles;
			sb_a.fu_array[i][j].busy = false;
			sb_b.fu_array[i][j].busy = false;
			sb_a.fu_array[i][j].fu_sn = fu_sn;
			sb_b.fu_array[i][j].fu_sn = fu_sn;
			fu_sn++;
		}
	}

	// Create instruction fetch buffer
	buffer = createQueue(INST_QUEUE_SIZE);
	if (buffer == NULL) {
		return -1;
	}
	// Create issued instruction queue
	sb_a.issued_buffer = createQueue(fu_count);
	sb_b.issued_buffer = createQueue(fu_count);
	if (sb_a.issued_buffer == NULL || sb_b.issued_buffer == NULL) {
		return -1;
	}
	
	// zero register results status array
	for (i = 0; i < REG_COUNT; i++) {
		sb_a.reg_res_status[i] = -1;
		sb_b.reg_res_status[i] = -1;
	}

	return 0;
}

void scoreboard_free() {
	int i;

	freeQueue(buffer);
	freeQueue(sb_a.issued_buffer);
	freeQueue(sb_b.issued_buffer);

	for (i = 0; i < FU_TYPES; i++) {
		free(sb_a.fu_array[i]);
		free(sb_b.fu_array[i]);
	}
}

void print_memout_regout(FILE *fp_memout,FILE *fp_regout) {
	int i,last, intpart;
	// count the max row number of memout file.
	last = MEM_SIZE - 1;
	while (last >= 0 && mem[last] == 0)
		last--;

	// print on memout file.
	for (i = 0; i <= last + 1; i++)
	{
		fprintf(fp_memout, "%08x\n", mem[i]);
	}

	// print on regout file.
	for (i = 0; i < 16; i++)
	{
		//intpart = (int)regs[i];
		fprintf(fp_regout, "%.6f\n", regs[i]);
	}
}

// Implementation of queue 

// function to create a queue of given capacity.  
// It initializes size of queue as 0 
Buffer* createQueue(unsigned capacity)
{
	struct Queue* queue = (Buffer*) malloc(sizeof(Buffer));
	queue->capacity = capacity;
	queue->front = queue->size = 0;
	queue->rear = capacity - 1;  // This is important, see the enqueue 
	queue->inst_array = (inst_status*)malloc(queue->capacity * sizeof(inst_status));
	return queue;
}

// Queue is full when size becomes equal to the capacity  
bool isFull(Buffer* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0 
bool isEmpty(Buffer* queue)
{
	return (queue->size == 0);
}

// Function to add an item to the queue.   
// It changes rear and size 
void enqueue(Buffer* queue, inst_status inst)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->inst_array[queue->rear] = inst;
	queue->size = queue->size + 1;
	//printf("%d enqueued to queue\n", inst);
}

// Function to remove an item from queue.  
// It changes front and size 
inst_status dequeue(Buffer* queue)
{
	if (isEmpty(queue)) {
		inst_status empty;
		empty.raw_inst = -1;
		return empty;
	}
	inst_status inst = queue->inst_array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return inst;
}

// Function to get front of queue 
inst_status front(Buffer* queue)
{
	if (isEmpty(queue)) {
		inst_status empty;
		empty.raw_inst = -1;
		return empty;
	}
	return queue->inst_array[queue->front];
}

// Function to get rear of queue 
inst_status rear(Buffer* queue)
{
	if (isEmpty(queue)) {
		inst_status empty;
		empty.raw_inst = -1;
		return empty;
	}
	return queue->inst_array[queue->rear];
}

// Function to free buffer memory
void freeQueue(Buffer* queue) {
	free(queue->inst_array);
	free(queue);
}




