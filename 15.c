#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;

int solve(FILE *in, size_t n);
// Returns the iterations done
int start(FILE *in, int *recentSpokenIndices);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in15", "r");

	printf("Part 1: %d\n", solve(in, 2020));
	rewind(in);
	printf("Part 2: %d\n", solve(in, 30000000));
	printf("Iterations: %lu\n", iterations);

	fclose(in);	
	return 0;	
}

int solve(FILE *in, size_t n)
{
	// 1-based
	int *recentSpokenIndices = calloc(n, sizeof(*recentSpokenIndices));
	int i = start(in, recentSpokenIndices);
	// Assuming the initial set didn't have any doubles
	int lastNumber = -1;
	for (; i <= n; i++)
	{
		iterations++;
		if (lastNumber == -1)
		{
			lastNumber = 0;
			continue;
		}

		int newNumber = recentSpokenIndices[lastNumber] != 0 
			? i - recentSpokenIndices[lastNumber] - 1
			: 0;

		recentSpokenIndices[lastNumber] = i - 1;
		lastNumber = newNumber;	
	}

	free(recentSpokenIndices);

	return lastNumber;
}

int start(FILE *in, int *recentSpokenIndices)
{	
	char *line = NULL;
	size_t n = 0;
	if (getline(&line, &n, in) < 0 || line[0] == '\0' || line[0] == '\n')
		return -1;

	char *token;
	char *lineIter = line;
	int i = 1;
	while ((token = strtok(lineIter, ",")) != NULL)
	{
		iterations++;
		int num = atoi(token);
		recentSpokenIndices[num] = i;

		lineIter = NULL;
		i++;
	}

	free(line);

	return i;
}
