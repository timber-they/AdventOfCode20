#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 100000
#define BITN 36
#define FLOAT_COUNT 1024

typedef unsigned long ulong;

typedef struct
{
	ulong address;
	ulong value;
} memVal;

size_t iterations = 0;

ulong emulate(FILE *in);
void parseMask(ulong *mask, char *line);
int getMemAddress(char *line);
ulong getValue(char *line);
ulong memSum(ulong *mem);
ulong emulate2(FILE *in);
void store(memVal *mem, ulong address, ulong value);
void storeWithMask(memVal *mem, ulong address, ulong value, char *mask);
ulong *getMaskedValues(ulong val, char *mask);
ulong getDefiniteMask(char *mask);
ulong setBit(int off, int bit, ulong val);
int **powerSet(int *set);
int *complement(int *set, int *sub);
ulong memSum2(memVal *mem);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in14", "r");

	ulong part1 = emulate(in);
	printf("Part 1: %lu\n", part1);

	rewind(in);
	ulong part2 = emulate2(in);
	printf("Part 2: %lu\n", part2);

	fclose(in);	
	return 0;	
}

ulong emulate2(FILE *in)
{
	char *mask = malloc(BITN * sizeof(*mask));
	memVal *mem = calloc(MEM_SIZE, sizeof(*mem));

	char *line = NULL;
	size_t n = 0;
	while (!(getline(&line, &n, in) < 0 || line[0] == '\0' || line[0] == '\n'))
	{
		*strchr(line, '\n') = '\0';
		if (strstr(line, "mask") != NULL)
		{
			memcpy(mask, strchr(line, '=')+2, BITN); 
			continue;
		}

		//printf("Mask: %s\n", mask);

		int address = getMemAddress(line);
		//printf("Got address: %d\n", address);
		ulong value = getValue(line);
		//printf("Got value: %lu\n", value);
		storeWithMask(mem, address, value, mask);
	}

	ulong sum = memSum2(mem);

	free(line);
	free(mem);
	free(mask);

	return sum;
}

void storeWithMask(memVal *mem, ulong address, ulong value, char *mask)
{
	ulong *maskValues = getMaskedValues(address, mask);
	//printf("Got masked values\n");
	for (int i = 0; maskValues[i] != 0; i++)
	{
		//printf("Storing %lu at %lu\n", value, maskValues[i]);
		store(mem, maskValues[i], value);
	}
}

ulong setBit(int off, int bit, ulong val)
{
	return bit ? val | (1l << off) : val & (~(1l << off));
}

ulong *getMaskedValues(ulong val, char *mask)
{
	ulong definite = getDefiniteMask(mask);
	int *floating = calloc(BITN, sizeof(*floating));
	ulong *res = calloc(FLOAT_COUNT, sizeof(*res));
	int fi = 0;

	val |= definite;
	for (int i = BITN-1; i >= 0; i--)
		if (mask[i] == 'X')
			floating[fi++] = BITN-i;

	//printf("Got some floatings\n");

	int **floatingPossibilities = powerSet(floating);
	//printf("Got a power set\n");
	for (int i = 0; floatingPossibilities[i] != NULL; i++)
	{
		int *set = floatingPossibilities[i];
		int *clear = complement(floating, set);
		ulong curr = val;
		for (int j = 0; set[j] != 0; j++)
		{
			//printf("\t\tSetting %d\n", set[j]-1);
			curr = setBit(set[j]-1, 1, curr);
		}
		for (int j = 0; clear[j] != 0; j++)
		{
			//printf("\t\tClearing %d\n", clear[j]-1);
			curr = setBit(clear[j]-1, 0, curr);
		}
		res[i] = curr;
		//printf("\tGot masked value %lu\n", res[i]);
	}

	return res;
}

int **powerSet(int *set)
{
	int **res = calloc(FLOAT_COUNT, sizeof(*res));
	res[0] = calloc(BITN, sizeof(*res[0]));
	if (*set == 0)
		// Empty set
		return res;

	int **sub = powerSet(set+1);
	for (int i = 0, j=0; sub[j] != NULL; j++)
	{
		// Without the first element
		res[i++] = sub[j];
		// With the first element
		res[i] = calloc(BITN, sizeof(*res[i]));
		res[i][0] = set[0];
		memcpy(res[i++]+1, sub[j], BITN-1);
	}

	return res;
}

int *complement(int *set, int *sub)
{
	int *res = calloc(BITN, sizeof(*res));
	int i = 0, k = 0;
	loop:
	for (; set[i] != 0; i++)
	{
		for (int j = 0; sub[j] != 0; j++)
			// Sub contains element
			if (sub[j] == set[i])
			{
				i++;
				goto loop;
			}
		// Sub doesn't contain element
		res[k++] = set[i];
	}

	return res;
}

void store(memVal *mem, ulong address, ulong value)
{
	for (int i = 0; i < MEM_SIZE; i++)
	{
		if (mem[i].address == 0)
		{
			//printf("Found free memory at %d\n", i);
			mem[i].address = address;
			mem[i].value = value;
			return;
		}
		if (mem[i].address == address)
		{
			//printf("Address %d was already known\n", address);
			mem[i].value = value;
			return;
		}
	}

	fprintf(stderr, "Too small memory\n");
	return;
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

	if ((token = strtok(NULL, "=")) == NULL)
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
ulong getDefiniteMask(char *mask)
{
	ulong res = 0;

	for (; *mask != '\0'; mask++)
	{
		if (*mask == ' ')
			continue;
		res = (res << 1) + (*mask == '1');
	}

	return res;
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

ulong memSum2(memVal *mem)
{
	ulong sum = 0;
	for (int i = 0; mem[i].address != 0; i++)
	{
		//printf("Summing %lu\n", mem[i].value);
		sum += mem[i].value;
	}
	return sum;
}
