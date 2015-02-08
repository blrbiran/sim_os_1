#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sim_os.h"


#define _DEBUG_FILENAME_	"sample2.txt"
#define _DEBUG_X_TIMER_		30
#define _DEBUG_



int mem[MEMORY_SIZE] = {0};
// int stack[STACK_SIZE] = {0};
// int sys_stack[STACK_SIZE] = {0};

void resetMemory(void)
{
	int i;
	for(i=0; i<MEMORY_SIZE; i++)
		mem[i] = 0;
}

// void resetStack(int *stack)
// {
	// int i;
	// for(i=0; i<STACK_SIZE; i++)
		// stack[i] = 0;
// }

void resetReg(SimReg *pReg)
{
	pReg->AC = 0;
	pReg->X = 0;
	pReg->Y = 0;
	pReg->SP = 0;
	pReg->PC = 0;
	pReg->IR = 0;
}

int read2memory(FILE *fp)
{
	char firstChar;
	int charType;
	int curMemoryPoint = 0;
	char tempLine[100];
	
	while((fgets(tempLine,100,fp)) != NULL)
	{
		firstChar = tempLine[0];
// #ifdef _DEBUG_
		// printf("%c", firstChar);
// #endif /* _DEBUG_ */
		charType = 0;
		
		if((firstChar >= '0') && (firstChar <= '9'))		// number
			charType = STR_NUM;
		else if(firstChar == '.')		// '.': address offset
			charType = STR_DOT;
		else if((firstChar == '\r') || (firstChar == '\n'))		// new line
			charType = STR_ENDLINE;
		else if(firstChar == ' ')		// space
			charType = STR_SPACE;
			
		switch(charType)
		{
		case STR_NUM:		// number
			sscanf(tempLine, "%d", mem+curMemoryPoint);
			
#ifdef _DEBUG_
			printf("LINE NUM=%d\t,CODE=%d\r\n", curMemoryPoint, mem[curMemoryPoint]);
#endif /* _DEBUG_ */
			curMemoryPoint++;
			break;
		case STR_DOT:		// '.': address offset
			sscanf(tempLine+1, "%d", &curMemoryPoint);
			break;
		case STR_ENDLINE:	// new line only surport UNIX and Mac file (Windows type end of line didn't work)
			// curMemoryPoint++;
			break;
		case STR_SPACE:	// new line only surport UNIX and Mac file (Windows type end of line didn't work)
			// curMemoryPoint++;
			break;
		default:
			printf("FILE_INPUT_ERROR\r\n");
			return FILE_INPUT_ERROR;
			
			break;
		}
	}
	
	printf("\r\nfinish read to memory\r\n");
	return OK;
}

int load_code(char *filename)
{
	int i;
	int status = 0;
	char char_temp[50];
	FILE *fp;
	if(fp=fopen(filename,"r"))		// file open success
	{
		puts("open file success\r\n");
	}
	else	// file open error
	{
		puts("open file failed\r\n");
		return FILE_OPEN_ERROR;
	}
	
	resetMemory();
	
	fseek(fp, 0L, SEEK_SET);	// reset file pointer
	
	status = read2memory(fp);
	if(status != OK)
		return status;
	
	
	fclose(fp);
	return OK;
}

void push_reg(SimReg *pReg, long *SP)
{
	mem[(*SP)--] = pReg->SP;
	mem[(*SP)--] = pReg->AC;
	mem[(*SP)--] = pReg->X;
	mem[(*SP)--] = pReg->Y;
	mem[(*SP)--] = pReg->PC;
	mem[(*SP)--] = pReg->IR;
}

void pop_reg(SimReg *pReg, long *SP)
{
	pReg->IR = mem[++(*SP)];
	pReg->PC = mem[++(*SP)];
	pReg->Y = mem[++(*SP)];
	pReg->X = mem[++(*SP)];
	pReg->AC = mem[++(*SP)];
	pReg->SP = mem[++(*SP)];
}

int get_random(void)
{
	int n;
	srand((unsigned)time(NULL));
	n=rand()%100 + 1;
	return n;
}

void mem_protection(int Addr, int sys_mode)
{
	if(sys_mode == 0)
	{
		if(Addr >= PROTECTION_ADDR)
		{
			printf("\r\nuser program cannot access system memory!\r\n");
			getchar();
			exit(0);
		}
	}
}

int run_code(SimReg *pReg, int TimerCnt)
{
	long Addr = 0;
	// long Code = 0;
	long Port = 0;
	int loopCnt = 0;
	int is_Interrupt = 0;		// 0:not in interrupt; 1:in int
	int sys_mode = 0;	// 0: user mode; 1:sys mode
	long sys_SP = 0;
	
	resetReg(pReg);
	
	pReg->SP = USER_STACK_ADDR;
	sys_SP = SYS_STACK_ADDR;
	// resetStack(stack);
	// resetStack(sys_stack);
	
	while((pReg->IR = mem[pReg->PC++]) != END)
	{
		switch(pReg->IR)
		{
		case LOAD_VALUE:	// Load the value into the AC
			pReg->AC = mem[pReg->PC++];
			break;
		case LOAD_ADDR:		// Load the value at the address into the AC
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			pReg->AC = mem[Addr];
			break;
		case LOADIND_ADDR:	// Load the value from the address found in the address into the AC
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			pReg->AC = mem[mem[Addr]];
			break;
		case LOADIDXX_ADDR:		// Load the value at (address+X) into the AC
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			pReg->AC = mem[Addr+(pReg->X)];
			break;
		case LOADIDXY_ADDR:		// Load the value at (address+X) into the AC
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			pReg->AC = mem[Addr+(pReg->Y)];
			break;
		case LOADSPX:		// Load from (Sp+X) into the AC
			pReg->AC = mem[(pReg->SP)+(pReg->X)];
			break;
		case STORE_ADDR:	// Store the value in the AC into the address
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			mem[Addr] = pReg->AC;
			break;
		case GET:	// Gets a random int from 1 to 100 into the AC
			pReg->AC = get_random();
			break;
		case PUT_PORT:	// If port=1, writes AC as an int to the screen; If port=2, writes AC as a char to the screen
			Port = mem[pReg->PC++];
			if(Port == 1)
				printf("%d", pReg->AC);
			else if(Port == 2)
				printf("%c", pReg->AC);
			break;
		case ADDX:		// Add the value in X to the AC
			pReg->AC += pReg->X;
			break;
		case ADDY:		// Add the value in Y to the AC
			pReg->AC += pReg->Y;
			break;
		case SUBX:		// Subtract the value in X from the AC
			pReg->AC -= pReg->X;
			break;
		case SUBY:		// Subtract the value in Y from the AC
			pReg->AC -= pReg->Y;
			break;
		case COPYTOX:	// Copy the value in the AC to X
			pReg->X = pReg->AC;
			break;
		case COPYFROMX:		// Copy the value in X to the AC
			pReg->AC = pReg->X;
			break;
		case COPYTOY:	// Copy the value in the AC to Y
			pReg->Y = pReg->AC;
			break;
		case COPYFROMY:		// Copy the value in Y to the AC
			pReg->AC = pReg->Y;
			break;
		
		case COPYFROMSP:		// Copy the value in SP to the AC
			pReg->AC = pReg->SP;
			break;
		case JUMP_ADDR:		// Jump to the address
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			pReg->PC = Addr;
			break;
		case JUMPIFEQUAL_ADDR:	// Jump to the address only if the value in the AC is zero
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			if(pReg->AC == 0)
				pReg->PC = Addr;
			break;
		case JUMPIFNOTEQUAL_ADDR:	// Jump to the address only if the value in the AC is not zero
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			if(pReg->AC != 0)
				pReg->PC = Addr;
			break;
		case CALL_ADDR:		// Push return address onto stack, jump to the address
			Addr = mem[pReg->PC++];
			mem_protection(Addr, sys_mode);
			mem[pReg->SP++] = pReg->PC;	// push onto stack
			pReg->PC = Addr;
			break;
		case RET:		// Pop return address from the stack, jump to the address
			pReg->PC = mem[--(pReg->SP)];	// pop from stack
			break;
		case INCX:		// Increment the value in X
			pReg->X++;
			break;
		case DECX:		// Decrement the value in X
			pReg->X--;
			break;
		case PUSH:		// Push AC onto stack
			mem[pReg->SP--] = pReg->AC;	// push onto stack
			break;
		case POP:		// Pop from stack into AC
			pReg->AC = mem[++(pReg->SP)];	// pop from stack
			break;
		case INT:		// Set system mode, switch stack, push SP and PC, set new SP and PC
			push_reg(pReg, &sys_SP);
			pReg->PC = SYS_CALL_ADDR;
			is_Interrupt = 1;
			sys_mode = 1;
			break;
		case IRET:		// Restore registers, set user mode
			pop_reg(pReg, &sys_SP);
			if(is_Interrupt == 1)
				is_Interrupt = 0;
			sys_mode = 0;
			break;
		default:		// default
			break;
		}
		loopCnt++;
		if((loopCnt >= TimerCnt) && (is_Interrupt == 0))
		{
			sys_mode = 1;
			loopCnt = 0;
			push_reg(pReg, &sys_SP);
			pReg->PC = TIMER_CALL_ADDR;
		}
	}
	
	return OK;
}

int main(int argc, char** argv)
{
	int errno = 0;	// error number
	char filename[100];	// store file name
	int TimerCnt = 0;
	SimReg Reg;
	SimReg *pReg = &Reg;
	
	if(argc <= 2)	// no argument, use default filename
	{
		strcpy(filename, _DEBUG_FILENAME_);
		printf("default load file:%s\r\n", filename);
		TimerCnt = _DEBUG_X_TIMER_;
	}
	else
	{
		strcpy(filename, argv[1]);
		sscanf(argv[2], "%d", &TimerCnt);
	}
	
	if(errno = load_code(filename))
	{
		printf("error code: %d\r\n", errno);
		getchar();
		return errno;
	}
	
	if(errno = run_code(pReg, TimerCnt))
	{
		printf("error code: %d\r\n", errno);
		getchar();
		return errno;
	}
	
	
	
	getchar();
	return OK;
}

