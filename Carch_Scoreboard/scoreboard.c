/*
Authors			- Nisan Tagar (302344031) Bat Sheva ()
Project Name	- Scoreboard Simmulator
Description		-
*/

#include "scoreboard.h"
float regs[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };	//Represent X16 32 bit single precision float Registers	

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
			fu_const_data[ADD_FU].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "sub_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[SUB_FU].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "mul_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[MUL_FU].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "div_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[DIV_FU].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "ld_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[LD_FU].available = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "st_nr_units") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ST_FU].available = temp_num;
			cfg_count++;
		}
		// Look for fu delay lines
		else if (strstr(tmp_str, "add_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ADD_FU].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "sub_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[SUB_FU].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "mul_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[MUL_FU].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "div_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[DIV_FU].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "ld_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[LD_FU].delay_cycles = temp_num;
			cfg_count++;
		}
		else if (strstr(tmp_str, "st_delay") != NULL) {
			temp_num = parse_line_number(tmp_str);
			if (temp_num == -1) {
				break;
			}
			fu_const_data[ST_FU].delay_cycles = temp_num;
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
	//if ()
}

void scoreboard_clk() {
	// 1) Issue one instruction 
	//op = bitSel(inst, 15, 12);
	//rd = bitSel(inst, 11, 8);
	//rs = bitSel(inst, 7, 4);
	//rt = bitSel(inst, 3, 0);
	//imm = 
// 2) Read operands
// 3) Execution
// 4) Write results
}

int scoreboard_init() {
	int i,j;
	// Create instruction buffer
	buffer = createQueue(INST_QUEUE_SIZE);
	if (buffer == NULL) {
		return -1;
	}
	
	// Allocate FU's 
	for (i = 0; i < FU_TYPES; i++) {
		sb_a.fu[i] = (fu_status*)malloc(fu_const_data[i].available * sizeof(fu_status));
		sb_a.fu_remaining[i] = fu_const_data[i].available;
		sb_b.fu[i] = (fu_status*)malloc(fu_const_data[i].available * sizeof(fu_status));
		sb_b.fu_remaining[i] = fu_const_data[i].available;
		if (sb_a.fu[i] == NULL || sb_b.fu[i] == NULL) {
			return -1;
		}
		// Freee all fu's of type i
		for (j = 0; j < fu_const_data[i].available; j++) {
			sb_a.fu[i][j].busy = false;
			sb_b.fu[i][j].busy = false;
		}
	}
	return 0;
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
int isFull(Buffer* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0 
int isEmpty(Buffer* queue)
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




