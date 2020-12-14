#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 100000

typedef unsigned long ulong;

size_t iterations = 0;

ulong emulate(FILE *in);
void parseMask(ulong *mask, char *line);
int getMemAddress(char *line);
ulong getValue(char *line);
ulong memSum(ulong *mem);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in14", "r");

	ulong part1 = emulate(in);
	printf("Part 1: %lu\n", part1);

	fclose(in);	
	return 0;	
}

ulong emulate(FILE *in)
{
	ulong *mask = calloc(2, sizeof(*mask));
	ulong *mem = calloc(MEM_SIZE, sizeof(*mem));

	char *line = NULL;
	size_t n = 0;
	while (!(getline(&line, &n, in) < 0 || line[0] == '\0' || line[0] == '\n'))
	{
		*strchr(line, '\n') = '\0';
		if (strstr(line, "mask") != NULL)
		{
			parseMask(mask, line);
			continue;
		}

		int address = getMemAddress(line);
		ulong value = getValue(line);
		mem[address] = value & mask[0] | mask[1];
	}

	ulong sum = memSum(mem);

	free(line);
	free(mask);
	free(mem);

	return sum;
}

void parseMask(ulong *mask, char *line)
{
	char *token = strtok(line, "=");
	line = NULL;

	if ((token = strtok(line, ",")) == NULL)
	{
		fprintf(stderr, "Invalid token for line %s\n", line);
		return;
	}

	mask[0] = 0, mask[1] = 0;
	for (; *token != '\0'; token++)
	{
		if (*token == ' ')
			continue;
		// And mask -> X and 1 become 1
		mask[0] = (mask[0] << 1) + (*token != '0');
		// Or mask -> Only 1 becomes 1
		mask[1] = (mask[1] << 1) + (*token == '1');
	}
}

int getMemAddress(char *line)
{
	char *start = strchr(line, '[') + 1;
	char *end = strchr(line, ']');
	*end = '\0';
	int res = atoi(start);
	*end = ']';
	return res;
}

ulong getValue(char *line)
{
	return atol(strchr(line, '=') + 2);
}

ulong memSum(ulong *mem)
{
	ulong sum = 0;
	for (int i = 0; i < MEM_SIZE; i++)
		sum += mem[i];
	return sum;
}
