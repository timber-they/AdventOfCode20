#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUB_CNT 3
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
int apply(rule r, char *line);
char **applyAll(rule r, char *lineptr, int depth);
int getValidCount(rule r, FILE *in);

int part2 = 0;

int main()
{
	FILE *in = fopen("in19", "r");

    rule *rules = parseRules(in);
    //for (int i = 0; i < RULE_CNT; i++)
        //printRule(rules[i]);
    rule rule0 = *rules;
    printf("Part 1: %d\n", getValidCount(rule0, in));
    //for (int i = 0; i < RULE_CNT; i++)
        //printRule(rules[i]);
    rewind(in);
    part2 = 1;
    rules = parseRules(in);
    //for (int i = 0; i < RULE_CNT; i++)
        //printRule(rules[i]);
    rule0 = *rules;
    printf("Part 2: %d\n", getValidCount(rule0, in));

	fclose(in);	
	return 0;	
}

void printRule(rule r)
{
    if (r.index < 0)
        return;
    printf("Rule %3d", r.index);
    if (r.atomic)
    {
        //printf("\tAtomic: %c\n", r.atomic);
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
        res += apply(r, line);
    }
    free(line);
    return res;
}

char **applyAll(rule r, char *lineptr, int depth)
{
    //if (r.index == 8 || r.index == 11 || r.index == 31 || r.index == 42)
        //printf("%d: Applying rule %d on %s\n", depth, r.index, lineptr);
    char **res = calloc(BRANCH_CNT, sizeof(*res));
    if (*lineptr == '\0')
    {
        //printf("Reached end of line\n");
        return res;
    }
    if (r.atomic)
    {
        //printf("Atomic: %c (%s)\n", r.atomic, lineptr);
        if (r.atomic == *lineptr)
            res[0] = lineptr+1;
        return res;
    }

    int resIndex = 0;
    for (int i = 0; i < SUB_CNT && r.sub[i] != NULL; i++)
    {
        char **curr = calloc(BRANCH_CNT, sizeof(*curr));
        curr[0] = lineptr;
        for (int j = 0; j < SUB_CNT && r.sub[i][j] != NULL; j++)
        {
            int newIndex = 0;
            char **new = calloc(BRANCH_CNT, sizeof(*new));
            for (int k = 0; curr[k] != NULL; k++)
            {
                //printf("Applying sub (%d, %d, %d)\n", i, j, k);
                char **sub = applyAll(*r.sub[i][j], curr[k], depth+1);
                //printf("Applied (%d, %d, %d) on %s\n", i, j, k, curr[k]);
                memcpy(new+newIndex, sub, (BRANCH_CNT - newIndex) * sizeof(*new));
                //printf("Copied - newIndex: %d (%s)\n", newIndex, new[newIndex]);
                for (; new[newIndex] != NULL; newIndex++);
                    //printf("Not null (%d) - %s\n", newIndex, new[newIndex]);
                //printf("Afterwards\n");
            }
            //printf("Updating curr\n");
            memset(curr, 0, BRANCH_CNT * sizeof(*curr));
            memcpy(curr, new, BRANCH_CNT * sizeof(*curr));
        }

        //printf("Done for one\n");

        memcpy(res+resIndex, curr, (BRANCH_CNT - resIndex) * sizeof(*res));
        for (; res[resIndex] != NULL; resIndex++);
    }
    return res;
}

int apply(rule r, char *line)
{
    //printf("Validating line %s\n", line);
    char **applied = applyAll(r, line, 0);
    for (int i = 0; applied[i] != NULL; i++)
    {
        if (applied[i][0] == '\0')
        {
            //printf("Line %s is valid\n", line);
            return 1;
        }
    }
    //printf("Line %s is invalid\n", line);
    return 0;
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
    //printf("Parsing\n");
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
    //printf("Parsed\n");
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
        //printRawRule(parsed);
    }
    //printf("Parsed everything\n");

    free(line);
    return res;
}

rawRule parseRule(char *line)
{
    if (part2 && line[0] == '8' && line[1] == ':')
        line = strdup("8: 42 | 42 8");
    else if (part2 && line[0] == '1' && line[1] == '1' && line[2] == ':')
        line = strdup("11: 42 31 | 42 11 31");
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

