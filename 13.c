#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUS_COUNT 100

size_t iterations = 0;
int getEarliestDeparture(FILE *in);
int getNextBus(int earliestDeparture, FILE *in);
int getWaitTime(long earliestDeparture, int busId);
int *getIds(FILE *in);
size_t naive2(int *ids);
int normalizedMax(int *ids);
size_t smart2(int *ids);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in13", "r");

	int earliestDeparture = getEarliestDeparture(in);
	printf("Earliest departure: %d\n", earliestDeparture);
	int nextBus = getNextBus(earliestDeparture, in);
	printf("Next bus: %d\n", nextBus);
	int waitTime = getWaitTime(earliestDeparture, nextBus);
	printf("Wait time: %d\n", waitTime);
	printf("=> Product: %d\n", nextBus * waitTime);

	rewind(in);
	// Ignore first line
	earliestDeparture = getEarliestDeparture(in);
	int *ids = getIds(in);
	printf("%ld\n", smart2(ids));

	free(ids);
	fclose(in);	
	return 0;	
}

int getNextBus(int earliestDeparture, FILE *in)
{
	char *line = NULL;
	size_t n = 0;
	int resId = -1, resWaitTime = -1;
	if (getline(&line, &n, in) < 0 || line[0] == '\0' || line[0] == '\n')
	{
		free(line);
		return -1;
	}

	char *token;
	while ((token = strtok(line, ",")) != NULL)
	{
		if (token[0] == 'x')
			continue;
		int id = atoi(token);
		int waitTime = getWaitTime(earliestDeparture, id);
		if (waitTime < resWaitTime || resWaitTime < 0)
		{
			resWaitTime = waitTime;
			resId = id;
		}

		line = NULL;
	}

	free(line);
	return resId;
}

int getEarliestDeparture(FILE *in)
{
	char *buff = NULL;
	size_t n = 0;
	int res = -1;
	if (!(getline(&buff, &n, in) < 0 || buff[0] == '\0' || buff[0] == '\n'))
		res = atoi(buff);

	free(buff);
	return res;
}

int getWaitTime(long earliestDeparture, int busId)
{
	return earliestDeparture % busId == 0 
		? 0 
		: (earliestDeparture / busId + 1) * busId - earliestDeparture;
}

int *getIds(FILE *in)
{
	char *line = NULL;
	size_t n = 0;
	if (getline(&line, &n, in) < 0 || line[0] == '\0' || line[0] == '\n')
	{
		free(line);
		return NULL;
	}

	int *res = calloc(BUS_COUNT, sizeof(*res));

	char *token;
	for (int i = 0; (token = strtok(line, ",")) != NULL; i++)
	{
		if (token[0] == 'x')
			continue;
		res[i] = atoi(token);
		line = NULL;
	}

	free(line);
	return res;
}

size_t naive2(int *ids)
{
	int incIndex = normalizedMax(ids);
	size_t t = 100000000000000 + getWaitTime(100000000000000, ids[incIndex]);
	printf("normalizedMax index: %d\n", incIndex);
	int maxI = 0;
	loop:
	t += ids[incIndex];
	for (int i = 0; i < BUS_COUNT; i++)
		if (ids[i] > 0 && (t+i-incIndex) % ids[i] != 0)
		{
			if (i > maxI)
			{
				maxI = i;
				printf("maxI: %d\n", maxI);
			}
			goto loop;
		}
	return t-incIndex;
}

int normalizedMax(int *ids)
{
	int max = 0, maxIndex = 0;
	for (int i = 0; i < BUS_COUNT; i++)
		if (ids[i] - i > max && ids[i] > 0)
		{
			max = ids[i] - i;
			maxIndex = i;
		}
	return maxIndex;
}

size_t findT(size_t mod, size_t start, size_t inc, size_t d)
{
	size_t t;
	for (t = start; t % mod != d; t+=inc)
		; // ignored
	return t;
}

size_t gcd(size_t a, size_t b)
{
	while (b != 0)
	{
		size_t t = b;
		b = a % b;
		a = t;
	}
	return a;
}

size_t lcm(size_t a, size_t b)
{
	return a * b / gcd(a,b);	
}

size_t smart2(int *ids)
{
	size_t start = 0;
	size_t inc = 1;
	for (int i = 0; i < BUS_COUNT; i++)
	{
		if (ids[i] <= 0)
			continue;
		long d = (ids[i] - i) % ids[i];
		while(d < 0)
			d += ids[i];
		start = findT(ids[i], start, inc, d);
		inc = lcm(inc, ids[i]);
	}
	return start;
}
