#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VALUE 1000
// I don't care about hardcoding
#define FIELD_COUNT 20

size_t iterations = 0;

typedef struct possibilityItem
{
	int index;
	int *possibilities;
} possibilityItem;

int getTicketScanningErrorRate(FILE *in);
void parseValidStates(char *line, int *validStates);
int *parseRule(char *rule);
int invalidFieldCount(char *ticket, int *validStates);
size_t part2(FILE *in);
int *parseTicket(char *line);
int *getFieldRanges(char *line);
possibilityItem *getPossibilities(int **impossibilities);
int compPossibilities(const void *a, const void *b);
int count(possibilityItem item);
int isInvalid(char *ticket, int *validStates);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in16", "r");

	int errorRate = getTicketScanningErrorRate(in);
	printf("Error rate: %d\n", errorRate);
	rewind(in);
	size_t res2 = part2(in);
	printf("Res 2: %ld\n", res2);

	fclose(in);	
	return 0;	
}

int getTicketScanningErrorRate(FILE *in)
{	
	char *line = NULL;
	size_t n = 0;
	// 0 -> rules, 1 -> your ticket, 2 -> nearby tickets
	int state = 0;
	int *validStates = calloc(MAX_VALUE, sizeof(*validStates));
	int res = 0;

	while (getline(&line, &n, in) >= 0 && line[0] != '\0' && line[0] != '\n' || state != 2 /* only 2 terminates */)
	{
		//printf("Got line %s", line);
		switch(state)
		{
			case 0:
				if(strstr(line, "your ticket") != NULL)
				{
					state = 1;
					break;
				}
				if (line[0] == '\n')
					break;
				parseValidStates(line, validStates);
				break;
			case 1:
				if (strstr(line, "nearby tickets") != NULL)
					state = 2;
				if (line[0] == '\n')
					break;
				break;
			case 2:
				res += invalidFieldCount(line, validStates);
				break;
			default:
				fprintf(stderr, "Invalid state: %d\n", state);
				return -1;
		}
	}


	free(validStates);
	free(line);
	return res;
}

size_t part2(FILE *in)
{
	char *line = NULL;
	size_t n = 0;
	// 0 -> rules, 1 -> your ticket, 2 -> nearby tickets
	int state = 0;
	int *validStates = calloc(MAX_VALUE, sizeof(*validStates));
	size_t res = 1;
	int *yourTicket = NULL;
	// The ith field name can't reference the jth field index, if impossibilities[i][j]
	int **impossibilities = malloc(FIELD_COUNT * sizeof(*impossibilities));
	int **fieldRanges = malloc(FIELD_COUNT * sizeof(*fieldRanges));
	int *containsDeparture = malloc(FIELD_COUNT * sizeof(*containsDeparture));
	int i = 0;
	int ticketIndex = 0;

	while (getline(&line, &n, in) >= 0 && line[0] != '\0' && line[0] != '\n' || state != 2 /* only 2 terminates */)
	{
		//printf("Got line %s", line);
		switch(state)
		{
			case 0:
				if(strstr(line, "your ticket") != NULL)
				{
					//printf("Done with 0\n");
					state = 1;
					break;
				}
				if (line[0] == '\n')
					break;
				parseValidStates(line, validStates);
				impossibilities[i] = calloc(FIELD_COUNT, sizeof(*impossibilities[i]));
				containsDeparture[i] = strstr(line, "departure") != NULL;
				//printf("i=%d; line=%s", i, line);
				fieldRanges[i] = getFieldRanges(line);
				i++;
				break;
			case 1:
				if (strstr(line, "nearby tickets") != NULL)
				{
					//printf("Done with 1\n");
					//for (i = 0; i < FIELD_COUNT; i++)
					//{
					//	for (int j = 0; j < MAX_VALUE; j++)
					//		printf("%d", fieldRanges[i][j] != 0);
					//	printf("\n");
					//}

					state = 2;
					break;
				}
				if (line[0] == '\n')
					break;
				yourTicket = parseTicket(line);
				break;
			case 2:
				if (isInvalid(line, validStates))
					// Ignore invalid tickets
					break;
				int *currentTicket = parseTicket(line);
				for (i = 0; i < FIELD_COUNT; i++)
				{
					//printf("%d, ", currentTicket[i]);
					for (int j = 0; j < FIELD_COUNT; j++)
					{
						if (impossibilities[i][j])
						{
							//printf("%d already can't be %d\n", i, j);
							continue;
						}
						if (!fieldRanges[i][currentTicket[j]])
						{
							//if (j == 17)
							//	printf("%d can't be %d (%d, by ticket %d)\n", i, j, currentTicket[j], ticketIndex);
							//printf("%d can't be %d\n", i, j);
							impossibilities[i][j] = 1;
						}
						//else if (i == 0 && j == 2)
						//{
						//	printf("%d can be %d (%d)\n", i, j, currentTicket[j]);
						//	if (currentTicket[j] < 31 || currentTicket[j] > 538 && currentTicket[j] < 546 || currentTicket[j] > 960)
						//		fprintf(stderr, "ERROR!\n");
						//}
					}
				}
				free(currentTicket);
				ticketIndex++;
				//printf("\n");
				break;
			default:
				fprintf(stderr, "Invalid state: %d\n", state);
				return -1;
		}
	}

	possibilityItem *possibilities = getPossibilities(impossibilities);
	int *identifiers = malloc(FIELD_COUNT * sizeof(*identifiers));
	for (i = 0; i < FIELD_COUNT; i++)
		identifiers[count(possibilities[i])] = i;
	qsort(possibilities, FIELD_COUNT, sizeof(*possibilities), &compPossibilities);
	int *done = calloc(FIELD_COUNT, sizeof(*done));
	for (i = 0; i < FIELD_COUNT; i++)
		done[i] = -1;

	int *assignments = malloc(FIELD_COUNT * sizeof(*assignments));

	for (i = 0; i < FIELD_COUNT; i++)
	{
		int j = 0;
		//printf("%d", possibilities[i].index);
		loop:
		for (; possibilities[i].possibilities[j] != -1; j++)
		{
			int k;
			for (k = 0; k < FIELD_COUNT; k++)
			{
				if (done[k] == -1)
					break;
				if (done[k] == possibilities[i].possibilities[j])
				{
					//printf("%d done ", done[k]);
					j++;
					goto loop;
				}
			}
			//printf("k=%d ", k);
			done[k] = possibilities[i].possibilities[j];
			//printf(" -> %d;", possibilities[i].possibilities[j]);
			assignments[possibilities[i].index] = possibilities[i].possibilities[j];
		}
		//printf(" (%d)\n", j);
	}

	for (i = 0; i < FIELD_COUNT; i++)
		if (containsDeparture[i])
			res *= yourTicket[assignments[i]];

	free(identifiers);
	for (i = 0; i < FIELD_COUNT; i++)
		free(possibilities[i].possibilities);
	free(possibilities);
	for (i = 0; i < FIELD_COUNT; i++)
		free(fieldRanges[i]);
	for (i = 0; i < FIELD_COUNT; i++)
		free(impossibilities[i]);
	free(impossibilities);
	for (i = 0; i < FIELD_COUNT; i++)
		free(fieldRanges);
	free(fieldRanges);
	free(yourTicket);
	free(done);
	free(line);
	free(containsDeparture);
	free(assignments);
	free(validStates);
	return res;	
}

int compPossibilities(const void *a, const void *b)
{
	possibilityItem p1 = *((possibilityItem *) a);
	possibilityItem p2 = *((possibilityItem *) b);
	return count(p1)-count(p2);
}

int count(possibilityItem item)
{
	int i;
	for (i = 0; i < FIELD_COUNT && item.possibilities[i] != -1; i++)
		; // Ignored	
	return i;
}

possibilityItem *getPossibilities(int **impossibilities)
{
	possibilityItem *res = malloc(FIELD_COUNT * sizeof(*res));
	for (int i = 0; i < FIELD_COUNT; i++)
	{
		res[i].index = i;
		res[i].possibilities = malloc((FIELD_COUNT+1) * sizeof(*(res[i].possibilities)));
		
		int k = 0;
		for (int j = 0; j < FIELD_COUNT; j++)
			if (!impossibilities[i][j])
				res[i].possibilities[k++] = j;
		for (; k < FIELD_COUNT+1; k++)
			res[i].possibilities[k] = -1;
	}

	return res;
}

void parseValidStates(char *line, int *validStates)
{
	char *dup = strdup(line);
	//printf("Parsing valid state %s", dup);
	char *rules = strchr(dup, ':');
	if (rules == NULL)
	{
		fprintf(stderr, "Couldn't parse rules %s", dup);
		free(dup);
		return;
	}

	rules = rules + 2;
	//printf("Got rules %s", rules);
	char *iter = rules;
	char *token;
	while ((token = strtok(iter, " ")) != NULL)
	{
		iter = NULL;
		if (strstr(token, "or") != NULL)
			// New rule
			continue;
		int *parsed = parseRule(token);
		//printf("Parsed to %d-%d\n", parsed[0], parsed[1]);
		memset(validStates+parsed[0], 1, (parsed[1]-parsed[0]+1) * sizeof(*validStates));
		free(parsed);
	}

	free(iter);
}

int *getFieldRanges(char *line)
{
	//printf("Getting field ranges of %s", line);
	int *fieldRanges = calloc(MAX_VALUE, sizeof(*fieldRanges));
	parseValidStates(line, fieldRanges);
	//printf("Parsed valid states\n");
	return fieldRanges;
}

int *parseRule(char *rule)
{
	//printf("Parsing rule %s\n", rule);
	char *split = strchr(rule, '-');
	*split = '\0';
	int *res = malloc(2 * sizeof(*res));
	res[0] = atoi(rule);
	res[1] = atoi(split+1);
	//printf("%d-%d\n", res[0], res[1]);
	return res;
}

int invalidFieldCount(char *ticket, int *validStates)
{
	char *dup = strdup(ticket);
	char *iter = dup;
	char *token;
	int res = 0;
	while ((token = strtok(iter, ",")) != NULL)
	{
		iter = NULL;
		int val = atoi(token);
		res += validStates[val] ? 0 : val;
	}

	free(dup);

	return res;
}

int isInvalid(char *ticket, int *validStates)
{
	char *dup = strdup(ticket);
	char *iter = dup;
	char *token;
	while ((token = strtok(iter, ",")) != NULL)
	{
		iter = NULL;
		if (!validStates[atoi(token)])
		{
			free(dup);
			return 1;
		}
	}

	free(dup);
	return 0;	
}

int *parseTicket(char *line)
{
	int *res = malloc(FIELD_COUNT * sizeof(*res));
	//printf("Parsing ticket %s", line);
	char *token = strtok(line, ",");
	for (int i = 0; token != NULL; token = strtok(line, ","), i++)
	{
		line = NULL;
		int val = atoi(token);
		//printf("Got val %d from token %s\n", val, token);
		res[i] = val;
	}

	return res;	
}
