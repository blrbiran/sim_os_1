#include <stdio.h>
#include <string.h>

#define OK					0
#define ARGUMENT_ERROR		1
#define FILE_OPEN_ERROR		2
#define FILE_EOF_ERROR		3
#define FILE_INPUT_ERROR	4

#define STR_NUM				1
#define STR_DOT				2
#define STR_ENDLINE			3

#define MEMORY_SIZE			2000

#define _DEBUG_FILENAME_	"sample1.txt"
#define _DEBUG_

int mem[MEMORY_SIZE] = {0};


void resetMemory(void)
{
	int i;
	for(i=0; i<MEMORY_SIZE; i++)
		mem[i] = 0;
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
#ifdef _DEBUG_
		printf("%c", firstChar);
#endif /* _DEBUG_ */
		charType = 0;
		
		if((firstChar >= '0') && (firstChar <= '9'))		// number
			charType = STR_NUM;
		else if(firstChar == '.')		// '.': address offset
			charType = STR_DOT;
		else if((firstChar == '\r') || (firstChar == '\n'))		// new line
			charType = STR_ENDLINE;
		
		switch(charType)
		{
		case STR_NUM:		// number
			sscanf(tempLine, "%d", mem+curMemoryPoint);
			
#ifdef _DEBUG_
			printf("\r\nLINE NUM=%d\t,CODE=%d\r\n", curMemoryPoint, mem[curMemoryPoint]);
#endif /* _DEBUG_ */
			curMemoryPoint++;
			break;
		case STR_DOT:		// '.': address offset
			sscanf(tempLine+1, "%d", &curMemoryPoint);
			break;
		case STR_ENDLINE:	// new line only surport UNIX and Mac file (Windows type end of line didn't work)
			curMemoryPoint++;
			break;
		default:
			printf("FILE_INPUT_ERROR\r\n");
			return FILE_INPUT_ERROR;
			
			break;
		}
	}
	
	printf("finish read to memory\r\n");
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
	
	fseek(fp, 0L, SEEK_SET);	// reset fp pointer
	
	status = read2memory(fp);
	if(status != OK)
		return status;
	
	
	fclose(fp);
	return OK;
}

int main(int argc, char** argv)
{
	int status = 0;
	char filename[100];
	
	if(argc <= 1)
	{
		// getchar();
		// return ARGUMENT_ERROR;
		strcpy(filename, _DEBUG_FILENAME_);
		printf("default load file:%s\r\n", filename);
	}
	else
	{
		strcpy(filename, argv[1]);
	}
	
	if(status = load_code(filename))
	{
		printf("error code: %d", status);
		getchar();
		return status;
	}
	
	
	getchar();
	return OK;
}

