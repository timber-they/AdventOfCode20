#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 1000
#define MAX_LINE 16
#define PREAMBLE 25

long *getNumbers(FILE *in);
int hasPairSum(long *set, int n, long goal);
long getFirstInvalid(long *numbers);
int *subsetSum(long *set, long goal);
long min(long *numbers, int n);
long max(long *numbers, int n);

size_t iterations = 0;

int main(int argc, char *argv[])
{
	FILE *in = fopen("in9", "r");

	long *numbers = getNumbers(in);
	long invalid = getFirstInvalid(numbers);
	printf("First invalid: %ld\n", invalid);

	int *subsetIndices = subsetSum(numbers, invalid);
	printf("sum_%d^%d=%ld\n",  subsetIndices[0], subsetIndices[1], invalid);
	long mi = min(numbers+subsetIndices[0], subsetIndices[1]-subsetIndices[0]);
	long ma = max(numbers+subsetIndices[0], subsetIndices[1]-subsetIndices[0]);
	printf("Min: %ld, max: %ld, sum: %ld\n", mi, ma, mi + ma);

	printf("Iterations: %ld\n", iterations);

	free(subsetIndices);
	free(numbers);
	fclose(in);	
	return 0;	
}

long *getNumbers(FILE *in)
{
	char *currentLine = malloc(MAX_LINE * sizeof(*currentLine));
	long *numbers = calloc(LINE_COUNT, sizeof(*numbers));
	for (int i = 0; i < LINE_COUNT; i++)
	{
		size_t n = MAX_LINE;
		if (getline(&currentLine, &n, in) < 0 || currentLine[0] == '\0' || currentLine[0] == '\n')
		{
			printf("Done (%s)\n", currentLine);
			free(currentLine);
			return numbers;
		}

		numbers[i] = atol(currentLine);
	}	

	return numbers;
}

long getFirstInvalid(long *numbers)
{
	for (int i = PREAMBLE; i < LINE_COUNT; i++)
		if (!hasPairSum(numbers+i-PREAMBLE, PREAMBLE, numbers[i]))
			return numbers[i];
	return -1;
}

int hasPairSum(long *set, int n, long goal)
{
	for (int i = 0; i < n-1; i++)
		for (int j = i+1; j < n; j++)
		{
			iterations++;
			if (set[i] + set[j] == goal)
				return 1;
		}
	return 0;
}

// My algorithms and datastructures teacher would not be happy with that solution
int *subsetSum(long *set, long goal)
{
	int *res = malloc(2 * sizeof(*res));
	for (res[0] = 0; res[0] < LINE_COUNT - 1; res[0]++)
	{
		long sum = set[res[0]];
		for (res[1] = res[0]+1; res[1] < LINE_COUNT && sum < goal; res[1]++)
		{
			iterations++;
			sum += set[res[1]];
		}
		if (sum == goal)
			return res;
	}

	free(res);
	return NULL;
}

long min(long *numbers, int n)
{
	if (n <= 0)
		return -1;
	long min = numbers[0];
	for (int i = 0; i < n; i++)
		if (numbers[i] < min)
			min = numbers[i];
	return min;
}
long max(long *numbers, int n)
{
	if (n <= 0)
		return -1;
	long max = numbers[0];
	for (int i = 0; i < n; i++)
		if (numbers[i] > max)
			max = numbers[i];
	return max;	
}
