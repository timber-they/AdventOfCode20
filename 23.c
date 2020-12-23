#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_COUNT 9
#define CUP_COUNT2 1000000

typedef struct Cups{
    int n;
    int max;
    int *cups;
    int *indices;
} Cups;

size_t iterations = 0;
// The cups are labelled with ASCII characters '0' - '9'
Cups *getCups(FILE *in);
Cups *getCups2(FILE *in);
// Start at 0
int playRound(Cups *cups, int last);
int get(Cups *cups, int i);
void set(Cups *cups, int i, int cup);
int getDestination(Cups *cups, int current, int destValue, int r1, int r2, int r3);
void play(Cups *cups, int rounds);
void printCups(Cups *cups);
void print2(Cups *cups);
int getIndex(Cups *cups, int cup);
void move(Cups *cups, int from, int to);
void move3(Cups *cups, int from, int to);
void printIndices(Cups *cups);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in23", "r");

    Cups *cups = getCups(in);
    play(cups, 100);
    printCups(cups);

    rewind(in);
    free(cups->cups);
    free(cups->indices);
    free(cups);
    printf("~\n");

    cups = getCups2(in);
    play(cups, 10000000/100);
    print2(cups);

    free(cups->cups);
    free(cups->indices);
    free(cups);
	fclose(in);	
	return 0;	
}

void printCups(Cups *cups)
{
    int i = getIndex(cups, 1);
    if (i == cups->n)
        printf("Couldn't find starting cup");
    else
        for (int j = 1; j < cups->n; j++)
            printf("%d", get(cups, j+i));
    printf("\n(");
    for (i = 0; i < cups->n; i++)
        printf("%d ", get(cups, i));
    printf(")\n");
}

void print2(Cups *cups)
{
    int i = getIndex(cups, 1);
    if (i == cups->n)
        printf("Couldn't find starting cup");
    else
    {
        int a = get(cups, i+1), b = get(cups, i+2);
        printf("%d * %d = %ld\n", a, b, ((long)a) * ((long)b));
    }
}

void play(Cups *cups, int rounds)
{
    int lastIndex = -1;
    int nextPrint = 1;
    for (int i = 0; i < rounds; i++)
    {
        if (i >= nextPrint)
        {
            printf("Round %d\n", i);
            if (nextPrint * 10 >= rounds)
                nextPrint += rounds / 10;
            else
                nextPrint *= 10;
        }
        //printf("=================\nRound %d (last %d)\n=================\n", i+1, lastIndex);
        //printCups(cups);
        //printIndices(cups);
        lastIndex = playRound(cups, lastIndex);
    }
}

int getIndex(Cups *cups, int cup)
{
    return cups->indices[cup];
} 

int playRound(Cups *cups, int lastIndex)
{
    int current = (lastIndex+1) % cups->n;
    int c = get(cups, current);
    //printf("c=%d\n", c);
    int destination = getDestination(cups, current, c-1, current+1, current+2, current+3);
    //printf("Current: %d (%d); (%d, %d, %d); destination: %d (%d)\n", c, current, get(cups, current+1), get(cups, current+2), get(cups, current+3), get(cups, destination), destination);
    //printCups(cups);
    move3(cups, current+1, destination+1);
    //printf("Played round...\n");
    current = getIndex(cups, c);
    return current;
}

int getDestination(Cups *cups, int current, int destValue, int r1, int r2, int r3)
{
    if (destValue < 1)
        destValue = cups->max;
    r1 = r1%cups->n, r2 = r2%cups->n, r3 = r3%cups->n;
    //printf("Looking for destination %d (not %d, %d, %d) [count: %d]\n", destValue, r1, r2, r3, cups->n);
    int destIndex = cups->indices[destValue];
    if (destIndex == r1 || destIndex == r2 || destIndex == r3)
        return getDestination(cups, current, destValue-1, r1, r2, r3);
    return getIndex(cups, destValue);
}

void move3(Cups *cups, int from, int to)
{
    from = from % cups->n;
    to = to % cups->n;
    int min = from;
    int max = to;
    if (to < min)
        min = to;
    if (from > max)
        max = from;
    move(cups, from, to);
    if (to < from)
    {
        to++;
        from++;
        from = from % cups->n;
        to = to % cups->n;
        if (to < min)
            min = to;
        if (from < min)
            min = from;
        if (to > max)
            max = to;
        if (from > max)
            max = from;
    }
    move(cups, from, to);
    if (to < from)
    {
        to++;
        from++;
        from = from % cups->n;
        to = to % cups->n;
        if (to < min)
            min = to;
        if (from < min)
            min = from;
        if (to > max)
            max = to;
        if (from > max)
            max = from;
    }
    move(cups, from, to);
    //for (int i = min; i <= max; i++)
        //cups->indices[cups->cups[i]] = i;
}

void move(Cups *cups, int from, int to)
{
    //printf("Moving from %d to %d\n", from, to);
    if (from == to)
        return;
    //int val = cups->cups[from];
    if (from < to)
    {
        to--;
        for (int i = 1; i <= cups->n; i++)
            if (cups->indices[i] > from && cups->indices[i] <= to)
                cups->indices[i]--;
            else if(cups->indices[i] == from)
                cups->indices[i] = to;
        //memmove(cups->cups+from, cups->cups+from+1, (to-from) * sizeof(*cups->cups));
    }
    else
    {
        for (int i = 1; i <= cups->n; i++)
            if (cups->indices[i] >= to && cups->indices[i] < from)
                cups->indices[i]++;
            else if(cups->indices[i] == from)
                cups->indices[i] = to;
        //memmove(cups->cups+to+1, cups->cups+to, (from-to) * sizeof(*cups->cups));
    }
    //cups->cups[to] = val;
    //printCups(cups);
}

void set(Cups *cups, int i, int cup)
{
    while (i < 0)
        i+=cups->n;
    //cups->cups[i%cups->n] = cup;
    cups->indices[cup] = i%cups->n;
}

int get(Cups *cups, int i)
{
    while(i < 0)
        i+=cups->n;
    i = i%cups->n;
    for (int j = 0; j <= cups->n; j++)
        if (cups->indices[j] == i)
            return j;
    printIndices(cups);
    fprintf(stderr, "Couldn't find %d\n", i);
    exit(1);
}

void printIndices(Cups *cups)
{
    for (int i = 0; i <= cups->n; i++)
        printf("%d->%d; ", i, cups->indices[i]);
    printf("\n");
}

Cups *getCups(FILE *in)
{
    int *res = calloc(CUP_COUNT, sizeof(*res));
    int *indices = calloc(CUP_COUNT+1, sizeof(*indices));
    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, in) <= 0 || *line == '\n' || *line == '\0')
    {
        fprintf(stderr, "Invalid input\n");
        free(line);
        free(res);
        exit(1);
    }
    indices[0] = -1;
    for (int i = 0; i < CUP_COUNT; i++)
    {
        res[i] = line[i] - '0';
        indices[res[i]] = i;
    }
    free(line);
    Cups *cups = calloc(1, sizeof(*cups));
    cups->cups = res;
    cups->indices = indices;
    cups->n = CUP_COUNT;
    cups->max = CUP_COUNT;
    //printf("Cup count: %d\n", cups->n);
    return cups;
}

Cups *getCups2(FILE *in)
{
    int *res = calloc(CUP_COUNT2, sizeof(*res));
    int *indices = calloc(CUP_COUNT2+1, sizeof(*res));
    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, in) <= 0 || *line == '\n' || *line == '\0')
    {
        fprintf(stderr, "Invalid input\n");
        free(line);
        free(res);
        exit(1);
    }
    indices[0] = -1;
    for (int i = 0; i < CUP_COUNT; i++)
    {
        res[i] = line[i] - '0';
        indices[res[i]] = i;
    }
    for (int i = CUP_COUNT; i < CUP_COUNT2; i++)
    {
        res[i] = i+1;
        indices[res[i]] = i;
    }
    free(line);
    Cups *cups = calloc(1, sizeof(*cups));
    cups->cups = res;
    cups->indices = indices;
    cups->n = CUP_COUNT2;
    cups->max = CUP_COUNT2;
    //printf("Cup count: %d\n", cups->n);
    return cups;
}

