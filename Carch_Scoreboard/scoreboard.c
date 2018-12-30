/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		-
*/

#include "scoreboard.h"
float regs[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };					//Represent X16 32 bit single precision float Registers	
char *fu_names[UNIT_NAME_SIZE] = { "LD","ST","ADD","SUB","MUL","DIV" };		// FU names, index corresponds to opcode number
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
	int temp_num,cfg_count=0;
	char tmp_str[20];

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
		new_inst.stage_cycle[EXEC] = -1;
		new_inst.stage_cycle[WRITE_RES] = -1;
		enqueue(buffer, new_inst);
		*pc = *pc + 1;
	}
	return;
}

void scoreboard_clk(unsigned int cc, bool *exitFlag_ptr, FILE *fp_trace_inst, FILE *fp_truce_unit) {
	inst_status next_inst;
	unsigned int free_fu;

	// Try to issue one instruction as long as halt flag is not raised and instruction present in buffer
	// Check if possible to issue instruction or if halt opperation
	if (isEmpty(buffer) == false && haltFlag==false) {
		next_inst = front(buffer);
		parse_inst(&next_inst);
		if (next_inst.opp == HALT_OP) {
			haltFlag = true; // TODO - Raise flag only when all remaining instructions graduated
		}
		if (availableFU(next_inst.opp, &free_fu) && sb_a.reg_res_status[next_inst.dst] == -1) {	
			// Issue procedure...
			sb_b.fu_array[next_inst.opp][free_fu].busy = true;		
			sb_b.fu_array[next_inst.opp][free_fu].f_i = next_inst.dst;
			if (next_inst.opp == LD_OP || next_inst.opp == ST_OP) {
				sb_b.fu_array[next_inst.opp][free_fu].f_j = -1;
				sb_b.fu_array[next_inst.opp][free_fu].f_k = -1;
			}
			else {
				sb_b.fu_array[next_inst.opp][free_fu].f_j = next_inst.src0;
				sb_b.fu_array[next_inst.opp][free_fu].f_k = next_inst.src1;
			}
			sb_b.fu_array[next_inst.opp][free_fu].q_j = sb_a.reg_res_status[next_inst.src0];
			sb_b.fu_array[next_inst.opp][free_fu].q_k = sb_a.reg_res_status[next_inst.src1];
			if (sb_a.reg_res_status[next_inst.src0] == -1)
				sb_b.fu_array[next_inst.opp][free_fu].r_j = true;
			else
				sb_b.fu_array[next_inst.opp][free_fu].r_j = false;

			if (sb_a.reg_res_status[next_inst.src1] == -1)
				sb_b.fu_array[next_inst.opp][free_fu].r_k = true;
			else
				sb_b.fu_array[next_inst.opp][free_fu].r_k = false;
			sb_b.reg_res_status[next_inst.dst] = sb_a.fu_array[next_inst.opp][free_fu].fu_sn;
			next_inst.stage_cycle[ISSUE] = cc;
			next_inst.issued_fu = free_fu;

			enqueue(sb_b.issued_buffer,next_inst);
			dequeue(buffer);
		}
	}

	// Check if any instructions currently issued
	if (isEmpty(sb_a.issued_buffer)) {
		if (haltFlag)
			*exitFlag_ptr = true;	// Issue queue is empty and halt command has been read
		return;
	}


	
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

void scoreboard_update() {
	int i;
	// Update FU status
	for (i = 0; i < FU_TYPES; i++) {
		sb_a.fu_array[i] = sb_b.fu_array[i];
	}
	
	// Update instruction status in issue queue
	sb_a.issued_buffer->capacity = sb_b.issued_buffer->capacity;
	sb_a.issued_buffer->front = sb_b.issued_buffer->front;
	sb_a.issued_buffer->rear = sb_b.issued_buffer->rear;
	sb_a.issued_buffer->size = sb_b.issued_buffer->size;

	for (i = 0; i < sb_b.issued_buffer->capacity; i++) {
		sb_a.issued_buffer->inst_array[i] = sb_b.issued_buffer->inst_array[i];
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
	int i,last;
	// count the max row number of memout file.
	last = MEM_SIZE - 1;
	while (last >= 0 && mem[last] == 0)
		last--;

	// print on memout file.
	for (i = 0; i <= last + 1; i++)
	{
		fprintf(fp_memout, "%08X\n", mem[i]);
	}

	// print on regout file.
	for (i = 0; i < 16; i++)
	{
		fprintf(fp_regout, "%08X\n", regs[i]);
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




