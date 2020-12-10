#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 2240
#define MAX_ANSWERS 26
#define MAX_PERSONS 10

char *getNormalizedGroup(FILE *in);
char **getDiscreteGroup(FILE *in);
void clear(char *line);
char *distinct(char *all);
char *intersect(char **all);

int main(void)
{
	FILE *in = fopen("in6", "r");

	char *group;
	char *dist;
	char **discreteGroup;
	char *inter;
	int sum = 0;
	while ((group = getNormalizedGroup(in)) != NULL)
	{
		dist = distinct(group);
		sum += strlen(dist);
		free(group);
		free(dist);
	}

	printf("Sum: %d\n", sum);

	rewind(in);

	sum = 0;
	while ((discreteGroup = getDiscreteGroup(in)) != NULL)
	{
		inter = intersect(discreteGroup);
		//printf("%s -> %s\n", *discreteGroup, inter);
		sum += strlen(inter);

		free(inter);
		for (int i = 0; discreteGroup[i] != NULL; i++)
		{
			free(discreteGroup[i]);
		}
		free(discreteGroup);
	}

	printf("Second sum: %d\n", sum);

	fclose(in);
	return 0;	
}

char *getNormalizedGroup(FILE *in)
{
	int c;
	char *res = calloc(MAX_ANSWERS * MAX_PERSONS, sizeof(*res));
	int i = 0;
	int gotLineBreak = 0;
	size_t currentRead = 0;

	while ((c = fgetc(in)) != EOF)
	{
		if (c == '\n')
		{
			if (gotLineBreak)
				return res;
			gotLineBreak = 1;
			continue;
		}
		gotLineBreak = 0;
		res[i++] = c;
	}

	return i > 0 ? res : NULL;
}

char **getDiscreteGroup(FILE *in)
{
	int c;
	char **res = calloc(MAX_PERSONS, sizeof(*res));
	*res = calloc(MAX_ANSWERS, sizeof(**res));
	int i = 0, j = 0;
	int gotLineBreak = 0;
	size_t currentRead = 0;

	while ((c = fgetc(in)) != EOF)
	{
		if (c == '\n')
		{
			if (gotLineBreak)
			{
				res[j] = NULL;
				return res;
			}
			gotLineBreak = 1;
			res[++j] = calloc(MAX_ANSWERS, sizeof(**res));
			i = 0;
			continue;
		}
		gotLineBreak = 0;
		res[j][i++] = c;
	}

	return i > 0 || j > 0 ? res : NULL;
}

char *distinct(char *all)
{
	char *res = calloc(strlen(all), sizeof(*res));
	for (int i = 0; *all != '\0'; all++)
	{
		if (strchr(res, *all) == NULL)
			res[i++] = *all;
	}
	return res;
}

char *intersect(char **all)
{
	char *res = calloc(MAX_ANSWERS, sizeof(*res));
	int i = 0, j = 0;
	loop:
	while((*all)[j] != '\0')
	{
		for (int k = 1; all[k] != NULL && strlen(all[k]) > 0; k++)
		{
			if (strchr(all[k], (*all)[j]) == NULL)
			{
				j++;
				goto loop;
			}
		}
		res[i++] = (*all)[j++];
	}

	return res;
}
