#ifndef __SIM_OS_H__
#define __SIM_OS_H__

#define OK					0
#define ARGUMENT_ERROR		1
#define FILE_OPEN_ERROR		2
#define FILE_EOF_ERROR		3
#define FILE_INPUT_ERROR	4

#define STR_NUM				1
#define STR_DOT				2
#define STR_ENDLINE			3
#define STR_SPACE			4

#define MEMORY_SIZE			2000
// #define STACK_SIZE			200

#define USER_STACK_ADDR		999
#define SYS_STACK_ADDR		1999

#define PROTECTION_ADDR		1000
#define TIMER_CALL_ADDR		1000
#define SYS_CALL_ADDR		1500

#define LOAD_VALUE				1		// Load the value into the AC
#define LOAD_ADDR				2		// Load the value at the address into the AC
#define LOADIND_ADDR			3		// Load the value from the address found in the address into the AC
#define LOADIDXX_ADDR			4		// Load the value at (address+X) into the AC
#define LOADIDXY_ADDR			5		// Load the value at (address+Y) into the AC
#define LOADSPX					6		// Load from (Sp+X) into the AC
#define STORE_ADDR				7		// Store the value in the AC into the address
#define GET						8		// Gets a random int from 1 to 100 into the AC
#define PUT_PORT				9		// If port=1, writes AC as an int to the screen; If port=2, writes AC as a char to the screen
#define ADDX					10		// Add the value in X to the AC
#define ADDY					11		// Add the value in Y to the AC
#define SUBX					12		// Subtract the value in X from the AC
#define SUBY					13		// Subtract the value in Y from the AC
#define COPYTOX					14		// Copy the value in the AC to X
#define COPYFROMX				15		// Copy the value in X to the AC
#define COPYTOY					16		// Copy the value in the AC to Y
#define COPYFROMY				17		// Copy the value in Y to the AC
#define COPYTOSP				18		// Copy the value in AC to the SP
#define COPYFROMSP				19		// Copy the value in SP to the AC 
#define JUMP_ADDR				20		// Jump to the address
#define JUMPIFEQUAL_ADDR		21		// Jump to the address only if the value in the AC is zero
#define JUMPIFNOTEQUAL_ADDR		22		// Jump to the address only if the value in the AC is not zero
#define CALL_ADDR				23		// Push return address onto stack, jump to the address
#define RET						24		// Pop return address from the stack, jump to the address
#define INCX					25		// Increment the value in X
#define DECX					26		// Decrement the value in X
#define PUSH					27		// Push AC onto stack
#define POP						28		// Pop from stack into AC
#define INT						29		// Set system mode, switch stack, push SP and PC, set new SP and PC
#define IRET					30		// Restore registers, set user mode
#define END						50		// End execution

typedef struct SimRegister
{
	long AC;
	long X;
	long Y;
	long SP;
	long PC;
	long IR;
} SimReg;


void resetMemory(void);
// void resetStack(int *stack);
void resetReg(SimReg *pReg);
int read2memory(FILE *fp);
int load_code(char *filename);
void push_reg(SimReg *pReg, long *SP);
void pop_reg(SimReg *pReg, long *SP);
int get_random(void);
void mem_protection(int Addr, int sys_mode);
int run_code(SimReg *pReg, int TimerCnt);


#endif /* __SIM_OS_H__ */
