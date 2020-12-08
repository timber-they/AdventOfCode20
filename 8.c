#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 638
#define MAX_LINE 10
#define INST_LENGTH 3

typedef enum Cmd
{
	acc,
	jmp,
	nop,
	eof
} Cmd;

typedef struct Inst
{
	Cmd cmd;
	int val;
} Inst;

Inst *read(FILE *in);
void printProgram(Inst *instructions);
// Returns the value of acc
int runProgram(Inst *instructions);
int contains(int *haystack, int needle);
int switchCmd(Inst *instruction);

// Hinting towards errors in the program execution
// -1 -> SEGFAULT
// 1  -> Loop
// 0  -> No error
int err = 0;

int main(int argc, char *argv[])
{
	FILE *in = fopen("in8", "r");
	Inst *instructions = read(in);
	int accRes = runProgram(instructions);

	printf("Acc after unmodified execution: %d\n", accRes);

	for (int i = 0; i < LINE_COUNT; i++)
	{
		if (!switchCmd(instructions+i))
			continue;

		accRes = runProgram(instructions);
		if (!err)
		{
			printf("Faulty command was at index %d, resulting in acc %d\n", i, accRes);
			break;
		}

		if (!switchCmd(instructions+i))
			fprintf(stderr, "Couldn't switch back at index %d\n", i);
	}

	free(instructions);
	fclose(in);	
	return 0;	
}

Inst *read(FILE *in)
{
	char *currentLine = malloc(MAX_LINE * sizeof(*currentLine));
	Inst *instructions = calloc(LINE_COUNT, sizeof(*instructions));
	for (int i = 0; i < LINE_COUNT; i++)
	{
		size_t n = MAX_LINE;
		if (getline(&currentLine, &n, in) < 0)
		{
			instructions[i].cmd = eof;
			free(currentLine);
			return instructions;
		}
		switch(currentLine[0])
		{
			case '\0':
				instructions[i].cmd = eof;
				free(currentLine);
				return instructions;
			case 'j':
				instructions[i].cmd = jmp;
				break;
			case 'a':
				instructions[i].cmd = acc;
				break;
			case 'n':
				instructions[i].cmd = nop;
				break;
			default:
				fprintf(stderr, "Command start %c (of line %s) was unexpected.\n", currentLine[0], currentLine);
				break;
		}

		// We're skipping the command and the sign
		instructions[i].val = atoi(currentLine+INST_LENGTH+2);
		instructions[i].val *= currentLine[INST_LENGTH+1] == '-' ? -1 : 1;
	}

	free(currentLine);
	return instructions;
}

void printProgram(Inst *instructions)
{
	for (int i = 0; i < LINE_COUNT; i++)
	{
		switch(instructions[i].cmd)
		{
			case acc:
				printf("acc");
				break;
			case jmp:
				printf("jmp");
				break;
			case nop:
				printf("nop");
				break;
			case eof:
				printf("eof");
				break;
		}

		printf(" %d\n", instructions[i].val);
	}
}

int runProgram(Inst *instructions)
{
	int *visittedIndices = calloc(LINE_COUNT, sizeof(*visittedIndices));
	memset(visittedIndices, -1, LINE_COUNT);
	int lpt = 0;
	// Visitted indices index
	int i = 0;
	int a = 0;

	err = 0;

	while (!contains(visittedIndices, lpt))
	{
		if (lpt >= LINE_COUNT)
		{
			// Segfault (oder so ähnlich)
			err = -1;
			free(visittedIndices);
			return a;
		}

		visittedIndices[i++] = lpt;

		switch(instructions[lpt].cmd)
		{
			case acc:
				a += instructions[lpt].val;
				lpt++;
				break;
			case jmp:
				lpt += instructions[lpt].val;
				break;
			case nop:
				lpt++;
				break;
			case eof:
				free(visittedIndices);
				return a;
		}
	}

	// Loop
	err = 1;
	free(visittedIndices);
	return a;
}

int contains(int *haystack, int needle)
{
	for (int i = 0; i < LINE_COUNT; i++)
	{
		if (haystack[i] == -1)
			// End of known values
			return 0;
		if (haystack[i] == needle)
			return 1;
	}

	return 0;
}

int switchCmd(Inst *instruction)
{
	switch(instruction->cmd)
	{
		case acc:
			return 0;
		case eof:
			return 0;
		case nop:
			instruction->cmd = jmp;
			return 1;
		case jmp:
			instruction->cmd = nop;
			return 1;
		default:
			fprintf(stderr, "Unexpected cmd: %d\n", instruction->cmd);
			return -1;
	}
}
