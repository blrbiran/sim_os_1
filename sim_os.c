#include <stdio.h>
#include <string.h>
#include "sim_os.h"


#define _DEBUG_FILENAME_	"sample4.txt"
#define _DEBUG_



long stack[STACK_SIZE] = {0};

int mem[MEMORY_SIZE] = {0};


void resetMemory(void)
{
	int i;
	for(i=0; i<MEMORY_SIZE; i++)
		mem[i] = 0;
}

void resetStack(void)
{
	int i;
	for(i=0; i<STACK_SIZE; i++)
		stack[i] = 0;
}

void resetReg(SimReg *pReg)
{
	pReg->AC = 0;
	pReg->X = 0;
	pReg->Y = 0;
	pReg->SP = 0;
	pReg->PC = 0;
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
			curMemoryPoint++;
			break;
		case STR_SPACE:	// new line only surport UNIX and Mac file (Windows type end of line didn't work)
			curMemoryPoint++;
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

int run_code(SimReg *pReg)
{
	resetReg(pReg);
	resetStack();
	
	
	
	return OK;
}

int main(int argc, char** argv)
{
	int errno = 0;	// error number
	char filename[100];	// store file name
	SimReg Reg;
	SimReg *pReg = &Reg;
	
	if(argc <= 1)	// no argument, use default filename
	{
		strcpy(filename, _DEBUG_FILENAME_);
		printf("default load file:%s\r\n", filename);
	}
	else
	{
		strcpy(filename, argv[1]);
	}
	
	if(errno = load_code(filename))
	{
		printf("error code: %d\r\n", errno);
		getchar();
		return errno;
	}
	
	
	
	
	getchar();
	return OK;
}

