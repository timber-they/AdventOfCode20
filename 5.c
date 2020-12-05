#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 10
#define LINE_COUNT 845

int parse(char *line);
void removeTrailingLineBreak(char *line);
void sort(int *in, int n);

int main(void)
{
	FILE *in = fopen("in5", "r");

	char *line = malloc((MAX_LINE + 1) * sizeof(*line));
	size_t n = MAX_LINE;

	int max = 0;
	int all[LINE_COUNT];
	int i = 0;

	while (getline(&line, &n, in) > 1)
	{
		removeTrailingLineBreak(line);
		int parsed = parse(line);
		if (parsed > max)
			max = parsed;
		all[i++] = parsed;
	}

	sort(all, LINE_COUNT);

	printf("Highest seat ID is: %d\n", max);

	for (int i = 1; i < LINE_COUNT; i++)
		if (all[i] != all[i-1] + 1)
			printf("%d -> %d => Suspect: %d\n", all[i-1], all[i], all[i-1]+1);

	free(line);
	fclose(in);
	return 0;	
}

int parse(char *line)
{
	int res = 0;
	int shift = strlen(line) - 1;
	for (; *line != '\0'; line++)
		res += (*line == 'B' || *line == 'R') << shift--;
	return res;
}

void removeTrailingLineBreak(char *line)
{
	for (char *pointer = line; *pointer != '\0'; pointer++)
		if (*pointer == '\n' || *pointer == '\r')
			*pointer = '\0';
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
		in[i+j] = j == (n/2 + n%2) || i != n/2 && a[i] < b[j] ? a[i++] : b[j++];

	free(a);
	free(b);
}
