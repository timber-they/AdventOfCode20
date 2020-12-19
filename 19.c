#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUB_CNT 2
#define RULE_CNT 138
#define BRANCH_CNT 100

size_t iterations = 0;

typedef struct rule
{
    char atomic;
    struct rule ***sub;
    int index;
} rule;

typedef struct rawRule
{
    char atomic;
    int **sub;
    int index;
} rawRule;

rule *parseRules(FILE *in);
rawRule parseRule(char *line);
rawRule *parseRawRules(FILE *in);
void printRawRule(rawRule rule);
void printRule(rule rule);
int apply(rule r, char **lineptr, int n);
int getValidCount(rule r, FILE *in);

int main()
{
	FILE *in = fopen("in19", "r");

    /*rawRule *raw = parseRawRules(in);
    printf("Parsed\n");
    for (int i = 0; i < RULE_CNT; i++)
    {
        printf("Rule %d:\n", i);
        printRawRule(raw[i]);
    }
    rewind(in);*/
    rule *rules = parseRules(in);
    //for (int i = 0; i < RULE_CNT; i++)
        //printRule(rules[i]);
    rule rule0 = *rules;
    printf("Part 1: %d\n", getValidCount(rule0, in));
    FILE *in2 = fopen("in19_2", "r");
    rules = parseRules(in);
    rule0 = *rules;
    printf("Part 2: %d\n", getValidCount(rule0, in2));

	fclose(in);	
    fclose(in2);
	return 0;	
}

void printRule(rule r)
{
    printf("Rule %3d", r.index);
    if (r.atomic)
    {
        printf("\tAtomic: %c\n", r.atomic);
        return;
    }

    printf("\t");
    for (int i = 0; i < SUB_CNT; i++)
    {
        if (r.sub[i] == NULL)
            break;
        if (i > 0)
            printf("| ");
        for (int j = 0; j < SUB_CNT; j++)
        {
            if (r.sub[i][j] == NULL)
                continue;
            printf("%d ", r.sub[i][j][0].index);
        }
    }
    printf("\n");
}

int getValidCount(rule r, FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    int res = 0;
    while (getline(&line, &n, in) > 0 && *line != '\n' && *line != '\0')
    {
        *strchr(line, '\n') = '\0';
        //printf("Validating %s\n", line);
        char *dup = strdup(line);
        char *dupPtr = dup;
        char **arr = calloc(BRANCH_CNT, sizeof(*arr));
        arr[0] = dup;
        res += apply(r, arr, 1) && **arr == '\0';
        free(dupPtr);
    }
    return res;
}

int apply(rule r, char **lineptr, int n)
{
    if (**lineptr == '\0')
        return 1;
    //printf("Applying %d to %s\n", r.index, *lineptr);
    if (r.atomic)
    {
        if (r.atomic == **lineptr)
        {
            //printf("Matched atomic\n");
            (*lineptr)++;
            return 1;
        }
        return 0;
    }

    int i = 0;
    int matched = 0;
loop:
    for (; i < SUB_CNT && r.sub[i] != NULL; i++)
    {
        char *dup = *lineptr;
        char **dupArr = calloc(BRANCH_CNT, sizeof(*dupArr));
        dupArr[0] = dup;
        for (int j = 0; j < SUB_CNT && r.sub[i][j] != NULL; j++)
        {
            //printf("{%d} (%d, %d) -> %s\n", r.index, i, j, dup);
            if (!apply(*r.sub[i][j], &dup, n))
            {
                i++;
                goto loop;
            }
        }
        //printf("Matched\n");
        lineptr[matched] = dup;
        matched++;
        break;
    }
    return matched > 0;
}

void printRawRule(rawRule rule)
{
    if (rule.atomic)
    {
        printf("\tAtomic: %c\n", rule.atomic);
        return;
    }

    printf("\t");
    for (int i = 0; i < SUB_CNT; i++)
    {
        if (rule.sub[i] == NULL)
            break;
        if (i > 0)
            printf("| ");
        for (int j = 0; j < SUB_CNT; j++)
        {
            if (rule.sub[i][j] == -1)
                continue;
            printf("%d ", rule.sub[i][j]);
        }
    }
    printf("\n");
}

rule *parseRules(FILE *in)
{
    rawRule *raw = parseRawRules(in);
    rule *res = calloc(RULE_CNT, sizeof(*res));
    for (int i = 0; i < RULE_CNT; i++)
    {
        res[i].index = raw[i].index;
        if (raw[i].atomic)
        {
            res[i].atomic = raw[i].atomic;
            continue;
        }
        
        if (raw[i].sub == NULL)
            continue;

        res[i].sub = calloc(SUB_CNT, sizeof(*(res[i].sub)));
        for (int j = 0; j < SUB_CNT; j++)
        {
            if (raw[i].sub[j] == NULL)
                continue;
            res[i].sub[j] = calloc(SUB_CNT, sizeof(*(res[i].sub[j])));
            for (int k = 0; k < SUB_CNT && raw[i].sub[j][k] >= 0; k++)
                res[i].sub[j][k] = res+raw[i].sub[j][k];
        }
    }
    return res;
}

rawRule *parseRawRules(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    rawRule *res = malloc(RULE_CNT * sizeof(*res));
    while (getline(&line, &n, in) > 0 && *line != '\n' && *line != '\0')
    {
        //printf("Raw line: %s", line);
        *strchr(line, '\n') = '\0';
        //printf("Line: %s\n", line);
        //printf("Parsing line %s\n", line);
        rawRule parsed = parseRule(line);
        //printf("Parsed - index: %d; atomic: %d\n", parsed.index, parsed.atomic);
        res[parsed.index] = parsed;
        //printf("Assgined\n");
    }
    //printf("Parsed everything\n");

    free(line);
    return res;
}

rawRule parseRule(char *line)
{
    rawRule res = {0};
    char *find = strchr(line, ':');
    *find = '\0';
    res.index = atoi(line);
    //printf("Got index %d\n", res.index);
    line = find+2;;

    find = strchr(line, '"');
    if (find != NULL)
    {
        //printf("Found atomic\n");
        res.atomic = find[1];
        return res;
    }

    res.sub = calloc(SUB_CNT, sizeof(*res.sub));
    //printf("Line: %s\n", line);
    find = strtok_r(line, "|", &line);
    for (int i = 0; find != NULL /*&& find[0] != '\0'*/; i++, find = strtok_r(NULL, "|", &line))
    {
        //printf("Find: '%s', %d\n", find, i);
        res.sub[i] = calloc(SUB_CNT, sizeof(*res.sub[i]));
        char *token;
        int j;
        token = strtok_r(find, " ", &find);
        for (j = 0; token != NULL && token[0] != '\0'; j++, token = strtok_r(NULL, " ", &find))
        {
            //printf("Token: %s; i: %d; j: %d\n", token, i, j);
            res.sub[i][j] = atoi(token);
            //printf("Assigned\n");
        }
        for (; j < SUB_CNT; j++)
        {
            //printf("Clearing %d\n", j);
            res.sub[i][j] = -1;
        }

        //printf("Onto the next!\n");
    }

    //printf("Aaaand we're done\n");

    return res;
}

