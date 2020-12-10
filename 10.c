#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 108
#define MAX_LINE 3

int *getNumbers(FILE *in);
void sort(int *in, int n);
int part1(int *in);
size_t part2(int *in);

size_t iterations = 0;

int main(int argc, char *argv[])
{
	FILE *in = fopen("in10", "r");

	int *numbers = getNumbers(in);
	sort(numbers, LINE_COUNT);

	int max = numbers[LINE_COUNT-1];
	int prod = part1(numbers);

	printf("Prod: %d\n", prod);
	size_t possibilities = part2(numbers);
	printf("Possibilities: %ld\n", possibilities);
	printf("Iterations: %ld\n", iterations);

	free(numbers);
	fclose(in);	
	return 0;	
}

size_t *knownPossibilities;
size_t getPossibilities(int i, int *in, int n)
{
	if (knownPossibilities[i] != 0)
		return knownPossibilities[i];

	iterations++;
	// i+1 must always work (see part1)
	size_t res = getPossibilities(i+1, in, n);
	if (i+2 < n && in[i+2] - in[i] < 4)
	{
		if (i == 10)
			printf("%d: %d - %d\n", i, in[i+2], in[i]);
		res += getPossibilities(i+2, in, n);
	}
	if (i+3 < n && in[i+3] - in[i] < 4)
		res += getPossibilities(i+3, in, n);

	knownPossibilities[i] = res;

	return res;
}

size_t part2(int *in)
{
	// 0 stands for unknown, as we can reach the end from each possible index (see part1)
	knownPossibilities = calloc((LINE_COUNT+2), sizeof(*knownPossibilities));
	knownPossibilities[LINE_COUNT+1] = 1;
	int *normalized = malloc((LINE_COUNT + 2) * sizeof(*normalized));
	normalized[0] = 0;
	normalized[LINE_COUNT + 1] = in[LINE_COUNT-1] + 3;
	memcpy(normalized+1, in, LINE_COUNT * sizeof(*normalized));

	return getPossibilities(0, normalized, LINE_COUNT+2);
}

int part1(int *in)
{
	// In the end one count3 adds
	int count1 = 0, count3 = 1;
	for (int i = 0; i < LINE_COUNT; i++)
	{
		iterations++;
		switch(in[i] - (i > 0 ? in[i-1] : 0))
		{
			case 1:
				count1++;
				break;
			case 2:
				break;
			case 3:
				count3++;
				break;
			default:
				fprintf(stderr, "Invalid diff at %d -> %d\n", i, in[i]);
				break;
		}
	}

	return count1 * count3;
}

int *getNumbers(FILE *in)
{
	char *currentLine = malloc(MAX_LINE * sizeof(*currentLine));
	int *numbers = calloc(LINE_COUNT, sizeof(*numbers));
	for (int i = 0; i < LINE_COUNT; i++)
	{
		iterations++;
		size_t n = MAX_LINE;
		if (getline(&currentLine, &n, in) < 0 || currentLine[0] == '\0' || currentLine[0] == '\n')
		{
			printf("Done (%s)\n", currentLine);
			free(currentLine);
			return numbers;
		}

		numbers[i] = atoi(currentLine);
	}	

	free(currentLine);
	return numbers;
}

// Mergesort
void sort(int *in, int n)
{
	if (n <= 1)
		// Already sorted
		return;
	int *a = malloc(n/2 * sizeof(*a));
	int *b = malloc((n/2 + n%2) * sizeof(*b));

	memcpy(a, in, n/2 * sizeof(*a));
	memcpy(b, in+n/2, (n/2 + n%2) * sizeof(*b));

	sort(a, n/2);
	sort(b, n/2 + n%2);

	for (int i = 0, j = 0; i < n/2 || j < n/2 + n%2;)
	{
		iterations++;
		in[i+j] = j == (n/2 + n%2) || i != n/2 && a[i] < b[j] ? a[i++] : b[j++];
	}

	free(a);
	free(b);
}
