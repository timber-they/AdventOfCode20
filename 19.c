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
    rule rule0 = *rules;
    printf("Part 1: %d\n", getValidCount(rule0, in));
    for (int i = 0; i < RULE_CNT; i++)
    {
        if (rules[i].sub == NULL)
            continue;
        for (int j = 0; j < SUB_CNT; j++)
            free(rules[i].sub[j]);
        free(rules[i].sub);
    }
    free(rules);
    rewind(in);
    part2 = 1;
    rules = parseRules(in);
    rule0 = *rules;
    printf("Part 2: %d\n", getValidCount(rule0, in));
    
    for (int i = 0; i < RULE_CNT; i++)
    {
        if (rules[i].sub == NULL)
            continue;
        for (int j = 0; j < SUB_CNT; j++)
            free(rules[i].sub[j]);
        free(rules[i].sub);
    }
    free(rules);
	fclose(in);	
	return 0;	
}

void printRule(rule r)
{
    if (r.index < 0)
        return;
    printf("Rule %3d", r.index);
    if (r.atomic)
        return;

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
        res += apply(r, line);
    }
    free(line);
    return res;
}

char **applyAll(rule r, char *lineptr, int depth)
{
    char **res = calloc(BRANCH_CNT, sizeof(*res));
    if (*lineptr == '\0')
        return res;
    if (r.atomic)
    {
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
                char **sub = applyAll(*r.sub[i][j], curr[k], depth+1);
                memcpy(new+newIndex, sub, (BRANCH_CNT - newIndex) * sizeof(*new));
                free(sub);
                for (; new[newIndex] != NULL; newIndex++);
            }
            memset(curr, 0, BRANCH_CNT * sizeof(*curr));
            memcpy(curr, new, BRANCH_CNT * sizeof(*curr));
            free(new);
        }

        memcpy(res+resIndex, curr, (BRANCH_CNT - resIndex) * sizeof(*res));
        free(curr);
        for (; res[resIndex] != NULL; resIndex++);
    }
    return res;
}

int apply(rule r, char *line)
{
    char **applied = applyAll(r, line, 0);
    for (int i = 0; applied[i] != NULL; i++)
        if (applied[i][0] == '\0')
        {
            free(applied);
            return 1;
        }
    free(applied);
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
    for (int i = 0; i < RULE_CNT; i++)
    {
        if (raw[i].sub == NULL)
            continue;
        for (int j = 0; j < SUB_CNT; j++)
            free(raw[i].sub[j]);
        free(raw[i].sub);
    }
    free(raw);
    return res;
}

rawRule *parseRawRules(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    rawRule *res = malloc(RULE_CNT * sizeof(*res));
    while (getline(&line, &n, in) > 0 && *line != '\n' && *line != '\0')
    {
        *strchr(line, '\n') = '\0';
        rawRule parsed = parseRule(line);
        res[parsed.index] = parsed;
    }

    free(line);
    return res;
}

rawRule parseRule(char *line)
{
    char *dupped = NULL;
    if (part2 && line[0] == '8' && line[1] == ':')
    {
        line = strdup("8: 42 | 42 8");
        dupped = line;
    }
    else if (part2 && line[0] == '1' && line[1] == '1' && line[2] == ':')
    {
        line = strdup("11: 42 31 | 42 11 31");
        dupped = line;
    }
    rawRule res = {0};
    char *find = strchr(line, ':');
    *find = '\0';
    res.index = atoi(line);
    line = find+2;;

    find = strchr(line, '"');
    if (find != NULL)
    {
        res.atomic = find[1];
        if (dupped)
            free(dupped);
        return res;
    }

    res.sub = calloc(SUB_CNT, sizeof(*res.sub));
    find = strtok_r(line, "|", &line);
    for (int i = 0; find != NULL /*&& find[0] != '\0'*/; i++, find = strtok_r(NULL, "|", &line))
    {
        res.sub[i] = calloc(SUB_CNT, sizeof(*res.sub[i]));
        char *token;
        int j;
        token = strtok_r(find, " ", &find);
        for (j = 0; token != NULL && token[0] != '\0'; j++, token = strtok_r(NULL, " ", &find))
            res.sub[i][j] = atoi(token);
        for (; j < SUB_CNT; j++)
            res.sub[i][j] = -1;
    }

    if (dupped)
        free(dupped);
    return res;
}

