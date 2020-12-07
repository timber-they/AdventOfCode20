#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define height 323
#define width 31
#define cases 5

void reset(char line[], int length);
void resetI(int line[], int length);

int main (int argc, char *argv[])
{
	FILE *in = fopen("in3", "r");

	/*
    Right 1, down 1.
    Right 3, down 1. (This is the slope you already checked.)
    Right 5, down 1.
    Right 7, down 1.
    Right 1, down 2.
	*/
	int down[cases] = {1,1,1,1,2};
	int right[cases] = {1,3,5,7,1};

	char currentLine[width+1];

	int y = 0;
	int x[cases];
	resetI(x, cases);
	int treeCount[cases];
	resetI(treeCount, cases);

	int prod = 1;

	// We start at (0,0), where no tree is
	fscanf(in, "%s\n", currentLine);
	reset(currentLine, width);

	while(fscanf(in, "%s\n", currentLine))
	{
		if (strlen(currentLine) < 30)
		{
			printf("Empty line - I reached my goal\n");
			break;
		}
		y++;

		for (int i = 0; i < cases; i++)
		{
			if (y % down[i] == 0)
			{
				x[i] = (x[i] + right[i]) % width;
				if (currentLine[x[i]] == '#')
					treeCount[i]++;
			}			
		}

		reset(currentLine, width);
	}

	for (int i = 0; i < cases; i++)
	{
		printf("Encountered %d trees for case %d.\n", treeCount[i], i);
		prod *= treeCount[i];
	}

	printf("Product: %d\n", prod);

	fclose(in);
	return 0;
}

void reset(char line[], int length)
{
	for (int i = 0; i < length; i++)
		line[i] = 0;
}

void resetI(int line[], int length)
{
	for (int i = 0; i < length; i++)
		line[i] = 0;
}
