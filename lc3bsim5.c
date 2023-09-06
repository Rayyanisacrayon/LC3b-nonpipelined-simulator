/*
    Name 1: Rayyan Zamir
    UTEID 1: rsz249
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    LD_PSR,
    PRIVMUX,
    GATE_PSR,
    GATE_VECTOR,
    LD_VECTOR,
    LD_EXC0,
    LD_EXC1,
    EXCMUX,
    LD_SSP,
    LD_USP,
    STACKMUX,
    GATE_SSP,
    GATE_USP,
    GATE_STACK,
    GATE_PC2,
    GATE_EXC,
    TR_ENABLE,
    GATE_J,
    PTE_MUX,
    GATE_PA,
    VA_ENABLE,
    LD_T,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetLD_PSR(int *x)        { return(x[LD_PSR]); }
int GetPRIVMUX(int *x)       { return(x[PRIVMUX]); }
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGATE_VECTOR(int *x)   { return(x[GATE_VECTOR]); }
int GetLD_VECTOR(int *x)     { return(x[LD_VECTOR]); }
int GetLD_EXC0(int *x)        { return(x[LD_EXC0]); }
int GetLD_EXC1(int *x)        { return(x[LD_EXC1]); }
int GetEXCMUX(int *x)         { return(x[EXCMUX]); }
int GetLD_SSP(int *x)        { return(x[LD_SSP]); }
int GetLD_USP(int *x)        { return(x[LD_USP]); }
int GetSTACKMUX(int *x)      { return(x[STACKMUX]); }
int GetGATE_SSP(int *x)      { return(x[GATE_SSP]); }
int GetGATE_USP(int *x)      { return(x[GATE_USP]); }
int GetGATE_STACK(int *x)    { return(x[GATE_STACK]); }
int GetGATE_PC2(int *x)      { return(x[GATE_PC2]); }
int GetGATE_EXC(int *x)      { return(x[GATE_EXC]); }
int GetTR_ENABLE(int *x)     { return(x[TR_ENABLE]); }
int GetGATE_J(int *x)        { return(x[GATE_J]); }
int GetPTE_MUX(int *x)       { return(x[PTE_MUX]); }
int GetGATE_PA(int *x)     { return(x[GATE_PA]); }
int GetVA_Enable(int *x)     { return(x[VA_ENABLE]); }
int GetLD_T(int *x)          { return(x[LD_T]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */
int interrupt_flag; 


typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

int PSR; /* Processor status register */
int SupervisorPtr; /* Pointer to supervisor stack */
int UserPtr; /* Pointer to user stack */
int Vector; /* Vector register */
int Op; /* invalid opcode reg*/
int Priv; /* Privilege violation reg */
int Align; /* Address align reg */

/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
/* MODIFY: you should add here any other registers you need to implement virtual memory */
int PF; /* Page fault reg */
int T; /* Trap detection reg */
int W; /* Write enable reg */
int JSTORE; /* j bit store reg */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;

  
  // generate a timed interrupt at cycle 300
  if(CYCLE_COUNT == 300)interrupt_flag = 1;

  //lower flag when interrupt is serviced
  if(CURRENT_LATCHES.STATE_NUMBER == 63)interrupt_flag = 0;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

/* MODIFY: you can add more initialization code HERE */
    CURRENT_LATCHES.SupervisorPtr = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.PSR = 0x8000;
    CURRENT_LATCHES.INTV = 0x01;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


int sext(int num, int len){
  int mask = 1 << (len - 1);
  if (num & mask){
    num = num | (0xFFFF << len);
  }
  return Low16bits(num);
}


void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
    */


   int msout = 0;

   // IRD Multiplexer **EDITED**
   if(GetGATE_J(CURRENT_LATCHES.MICROINSTRUCTION)){
        msout = CURRENT_LATCHES.JSTORE;
   }
   else if((CURRENT_LATCHES.Priv || CURRENT_LATCHES.Align || CURRENT_LATCHES.PF) && GetGATE_EXC(CURRENT_LATCHES.MICROINSTRUCTION)){
        msout = 63;
   }
   else if(GetTR_ENABLE(CURRENT_LATCHES.MICROINSTRUCTION)){
        msout = 52;
   }
   else if(GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)){
        msout = (CURRENT_LATCHES.IR & 0xf000) >> 12;
   }
   else{
        //grab j bits
        msout = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);

        //grab cond bits **EDITED**
        int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
        // 101 case -> Interrupt
        if(cond == 5){
            msout |= ((interrupt_flag << 4));
        }
        // 100 case -> PSR[15]
        else if(cond == 4){
            msout |= ((CURRENT_LATCHES.PSR & 0x8000) >> 12);
        }
        // 011 case -> IR[11]
        if(cond == 3){
            msout |= ((CURRENT_LATCHES.IR & 0x0800) >> 11);
        }
        // 010 case -> BEN
        else if(cond == 2){
            msout |= (CURRENT_LATCHES.BEN << 2);
        }
        // 001 case -> READY
        else if(cond == 1){
            msout |= (CURRENT_LATCHES.READY << 1);
        }
   }
   msout = Low16bits(msout);
   NEXT_LATCHES.STATE_NUMBER = msout;
   memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[msout], sizeof(int)*CONTROL_STORE_BITS);
}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */

    static int cycle_count = 0;
    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) && !GetPTE_MUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.READY = 0;
        cycle_count++;
        if (cycle_count == (MEM_CYCLES - 1)){
            NEXT_LATCHES.READY = 1;
            
            //when in read mode
            if (!GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) && GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
                if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)){
                    NEXT_LATCHES.MDR = (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) | MEMORY[CURRENT_LATCHES.MAR >> 1][0];
                }
                else{
                    NEXT_LATCHES.MDR = MEMORY[CURRENT_LATCHES.MAR >> 1][CURRENT_LATCHES.MAR & 0x1];
                }
                NEXT_LATCHES.MDR = Low16bits(NEXT_LATCHES.MDR);
            }
            //when in write mode
            else {
                if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)){
                    MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR & 0xff00) >> 8;
                    MEMORY[CURRENT_LATCHES.MAR >> 1][0] = CURRENT_LATCHES.MDR & 0x00ff;
                }
                else{
                    MEMORY[CURRENT_LATCHES.MAR >> 1][CURRENT_LATCHES.MAR & 0x1] = CURRENT_LATCHES.MDR & 0x00ff;
                }
            }
        }
    }
    else{
        cycle_count = 0;
    }

}

int marmux_out;
int alu_out;
int shf_out;
int logic_out;
int adder_out;
int stackptr;
int vec2;


int sr2_mux(int ir, int sr2){
    if ((ir >> 5) & 0x1){
        return sext((ir & 0x1f), 5);
    }
    else{
        return sr2;
    }
}

int marmux(int ir, int adder_out){
    if (!GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        int out = (ir & 0x00ff) << 1;
        return out;
    } 
    else{
        return adder_out;   
    }
}
int address_adder(int addr2mux_out, int addr1mux_out){
    if(GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION)){
        addr2mux_out = addr2mux_out << 1;
    }
    int out = addr2mux_out + addr1mux_out;
    return out;
}

int addr1mux(int sr1out, int pc){
    if(!GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        return pc;
    }
    else{
        return sr1out;
    }
}

int addr2mux(int ir){
    int out = 0;
    //00 case
    if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        out = 0;
    }
    //01 case
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        out = sext(ir & 0x003f, 6);
    }
    //10 case
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        out = sext(ir & 0x01ff, 9);
    }
    //11 case
    else if (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
        out = sext(ir & 0x07ff, 11);
    }
    return out;
}

int alu(int a, int b){
    int aluk = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
    int out = 0;
    if(aluk == 0){
        out = a + b;
    }
    else if(aluk == 1){
        out = a & b;
    }
    else if(aluk == 2){
        out = a ^ b;
    }
    else{
        out = a;
    }
    return out;
}

int shifter(int sr1, int ir){
    //lshf
    if(!(ir & 0x0030)){
        return sr1 << (ir & 0x000f);    }
    //rshfl
    else if((ir & 0x0030) == 0x0010){
        int temp = sr1 >> (ir & 0x000f);
        //mask out the bits that were shifted out
        int mask = 0xFFFF << (16 - (ir & 0xF));
        mask = Low16bits(mask);
        temp = temp & ~mask; 
        temp = Low16bits(temp);
        return temp;
    }
    //arithmatic right shift
    else if((ir & 0x0030) == 0x0030){
        int temp = sr1 >> (ir & 0x0F);
        int mask = 0xFFFF << (16 - (ir & 0xF) - 1);
        mask = Low16bits(mask);
        temp = temp & ~mask;
        if (sr1 & 0x8000){
            temp = temp | mask;
        }
        temp = Low16bits(temp);

        return temp;
    }
    else{
        return 0;
    }
}

int logic(int mar0, int datasize, int input){
    if(datasize == 1){
        return input;
    }
    else{
        return sext((input & 0x00ff), 8);
    }
}

//sr1mux
int find_sr1(int ir){ //**EDITED**
    if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return CURRENT_LATCHES.REGS[6];
    }
    else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return CURRENT_LATCHES.REGS[(ir & 0x01C0) >> 6];
    }
    else{
        return CURRENT_LATCHES.REGS[(ir & 0x0E00) >> 9];
    }
}

//drmux
int find_dr(int ir){ //**EDITED**
    if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return 6;
    }
    else if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return 7;
    }
    else{
        return ((ir & 0x0E00) >> 9);
    }
}


// Added
int vectormux(int p, int pf, int a){
    if(!p && !pf && !a){
        return CURRENT_LATCHES.INTV;
    }
    else if(a){
        return 0x03;
    }
    else if(p){
        return 0x04;
    }
    else{
        return 0x02;
    }
}

int vec2mux(int op, int vec1out){
    if(op){
        return 0x05 << 1;
    }
    else{
        return vec1out << 1;
    }
}

int stackmux(int r6, int smux){
    if(smux){
        return r6 - 2;
    }
    else{
        return r6 + 2;
    }
}


void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *         Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
    int sr1 = find_sr1(CURRENT_LATCHES.IR);
    int adder1_out = addr1mux(sr1, CURRENT_LATCHES.PC);
    int adder2_out = addr2mux(CURRENT_LATCHES.IR);
    adder_out = address_adder(adder2_out, adder1_out);
    marmux_out = marmux(CURRENT_LATCHES.IR, adder_out);
    int sr2mux_out = sr2_mux(CURRENT_LATCHES.IR, CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0007)]);
    alu_out = alu(sr1, sr2mux_out);
    shf_out = shifter(sr1, CURRENT_LATCHES.IR);
    logic_out = logic(CURRENT_LATCHES.MAR & 0x0001, GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION), CURRENT_LATCHES.MDR);
    int vec1out = vectormux(CURRENT_LATCHES.Priv, CURRENT_LATCHES.PF, CURRENT_LATCHES.Align);
    vec2 = vec2mux(CURRENT_LATCHES.Op, vec1out);
    stackptr = stackmux(sr1, GetSTACKMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       
    if(GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = marmux_out;
    }
    else if(GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.PC;
    }
    else if(GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = alu_out;
    }
    else if(GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = shf_out;
    }
    else if(GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = logic_out;
    }
    else if(GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.Vector;
    }
    else if(GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.SupervisorPtr;
    }
    else if(GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.UserPtr;
    }
    else if(GetGATE_STACK(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = stackptr;
    }
    else if(GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = (CURRENT_LATCHES.PSR & 0xFFF8) | (CURRENT_LATCHES.P) | (CURRENT_LATCHES.N << 2) | (CURRENT_LATCHES.Z << 1);
    }
    else if(GetGATE_PC2(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.PC - 2;
    }
    else if(GetVA_Enable(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = (CURRENT_LATCHES.PTBR & 0xFF00) | ((CURRENT_LATCHES.MAR & 0xFE00) >> 8);
    }
    else if(GetGATE_PA(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = ((CURRENT_LATCHES.VA & 0x01FF) | (CURRENT_LATCHES.MDR & 0x3E00));
    }
    else{
        BUS = 0;
    }
    BUS = Low16bits(BUS);
}

int pc_mux(int bus, int pc, int adder_out){
    if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        return pc + 2;
    }
    else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        return bus;
    }
    else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
        return adder_out;
    }
    else if (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
        return 0;
    }
}
void setcc(int input){
        
        if(input == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if(input & 0x8000){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
}

//functions to set exception bits
int a_exc(int bus, int datasize){
    if((bus & 0x0001) && datasize){
        return 1;
    }
    else{
        return 0;
    }
}

int p_exc(int signal0){
    if(GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        return ((!(signal0 & 0x0004) >> 2) && CURRENT_LATCHES.T);
    }
    else{
        return 0;
    }
}

int o_exc(int opmux){
    if(opmux){
        return 1;
    }
    else{
        return 0;
    }
}

int pf_exc(int signal0){
    if(GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        return (!(signal0 & 0x0002) >> 1);
    }
    else{
        return 0;
    }
}



void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       
    int dr = find_dr(CURRENT_LATCHES.IR);
    int sr1 = find_sr1(CURRENT_LATCHES.IR);
    int signal0 = (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) | MEMORY[CURRENT_LATCHES.MAR >> 1][0];

    //latch pc
    int pcmux_out = pc_mux(BUS, CURRENT_LATCHES.PC, adder_out);
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.PC = Low16bits(pcmux_out);
    }
    //latch reg
    if(GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.REGS[dr] = Low16bits(BUS);
    }
    //latch mar
    if(GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.MAR = Low16bits(BUS);
    }
    //latch ir
    if(GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    //latch ben
    if(GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(CURRENT_LATCHES.N && CURRENT_LATCHES.IR & 0x0800){
            NEXT_LATCHES.BEN = 1;
        }
        else if(CURRENT_LATCHES.Z && CURRENT_LATCHES.IR & 0x0400){
            NEXT_LATCHES.BEN = 1;
        }
        else if(CURRENT_LATCHES.P && CURRENT_LATCHES.IR & 0x0200){
            NEXT_LATCHES.BEN = 1;
        }
        else{
            NEXT_LATCHES.BEN = 0;
        }
    }
    //latch cc **EDITED**
    if(GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
            NEXT_LATCHES.N = (BUS & 0x0004 >> 2);
            NEXT_LATCHES.Z = (BUS & 0x0002 >> 1);
            NEXT_LATCHES.P = (BUS & 0x0001);
        }
        else{
            setcc(BUS);
        }
    }

    //latch mdr
    if(GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION) && GetPTE_MUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.MDR = ((CURRENT_LATCHES.MDR | 0x01) | ((CURRENT_LATCHES.W & 0x01) << 1));
    }
    else if(!GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) && GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
        logic_out = logic(CURRENT_LATCHES.MAR & 0x0001, GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION), Low16bits(BUS));
        NEXT_LATCHES.MDR = Low16bits(logic_out);
    }


    //NEW
    //latch exceptions
    if(GetLD_EXC0(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.Align = a_exc(BUS, GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION));
        NEXT_LATCHES.Op = o_exc(GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    }
    if(GetLD_EXC1(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.Priv = p_exc(signal0);
        NEXT_LATCHES.PF = pf_exc(signal0);
    }
    //latch vector
    if(GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.Vector = Low16bits(vec2 + 0x0200);
    }

    //latch usp
    if(GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.UserPtr = Low16bits(sr1);
    }

    //latch ssp
    if(GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.SupervisorPtr = Low16bits(sr1);
    }

    //latch psr
    if(GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetPRIVMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
            NEXT_LATCHES.PSR = BUS & 0x7FFF;
        }
        else{
            NEXT_LATCHES.PSR = Low16bits(BUS);
        }
    }

    //lab 5
    //latch VA
    if(GetVA_Enable(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
    }
    
    //latch W, Jstore
    if(GetTR_ENABLE(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.W = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
        NEXT_LATCHES.JSTORE = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    }

    //latch T
    if(GetLD_T(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.T = GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    }
}