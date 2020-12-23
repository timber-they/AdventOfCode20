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
void insert(Cups *cups, int i, int cup);
// Returns the removed cup
int rem(Cups *cups, int i);
int getDestination(Cups *cups, int current, int destValue, int r1, int r2, int r3);
void play(Cups *cups, int rounds);
void printCups(Cups *cups);
void print2(Cups *cups);
int getIndex(Cups *cups, int cup, int guess);
void move(Cups *cups, int from, int to);

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
    play(cups, 10000000);
    print2(cups);

    free(cups->cups);
    free(cups->indices);
    free(cups);
	fclose(in);	
	return 0;	
}

void printCups(Cups *cups)
{
    int i = getIndex(cups, 1, 100);
    if (i == cups->n)
        printf("Couldn't find starting cup");
    else
        for (int j = 1; j < cups->n; j++)
            printf("%d", get(cups, j+i));
    printf("\n(");
    for (i = 0; i < cups->n; i++)
        printf("%d ", cups->cups[i]);
    printf(")\n");
}

void print2(Cups *cups)
{
    int i = getIndex(cups, 1, 100);
    if (i == cups->n)
        printf("Couldn't find starting cup");
    else
    {
        int a = get(cups, i+1), b = get(cups, i+2);
        printf("%d * %d = %d\n", a, b, a*b);
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
            nextPrint *= 10;
        }
        printf("=================\nRound %d (last %d)\n=================\n", i+1, lastIndex);
        printCups(cups);
        lastIndex = playRound(cups, lastIndex);
    }
}

int getIndex(Cups *cups, int cup, int guess)
{
    //return cups->indices[cup];
    int i;
    if (guess < 0)
        guess = 8;
    //printf("Guess: %d\n", guess);
    for (i = guess < cups->n ? guess : cups->n-1; i >= 0; i--)
        if (cups->cups[i] == cup)
            return i;
    //printf("Invalid guess for %d!!! ", cup);
    for (i = cups->n-1; i > guess; i--)
        if (cups->cups[i] == cup)
        {
            //printf("Actual index was %d\n", i);
            return i;
        }
    exit(1);
} 

int playRound(Cups *cups, int lastIndex)
{
    int current = (lastIndex+1) % cups->n;
    int c = get(cups, current);
    //printf("c=%d\n", c);
    int r1 = get(cups, current+1);
    int r2 = get(cups, current+2);
    int r3 = get(cups, current+3);
    int destination = getDestination(cups, current, c-1, r1, r2, r3);
    printf("Current: %d (%d); (%d, %d, %d); destination: %d (%d)\n", c, current, r1, r2, r3, get(cups, destination), destination);
    //printCups(cups);
    move(cups, current+1, destination+1);
    move(cups, current+1, destination+1);
    move(cups, current+1, destination+1);
    //printf("Played round...\n");
    current = getIndex(cups, c, current-1);
    return current;
}

int getDestination(Cups *cups, int current, int destValue, int r1, int r2, int r3)
{
    if (destValue < 1)
        destValue = cups->max;
    //printf("Looking for destination %d (not %d, %d, %d) [count: %d]\n", destValue, r1, r2, r3, cups->n);
    if (destValue == r1 || destValue == r2 || destValue == r3)
        return getDestination(cups, current, destValue-1, r1, r2, r3);
    return getIndex(cups, destValue, destValue+10);
}

void insert(Cups *cups, int i, int cup)
{
    //printf("Inserting %d at %d\n", cup, i);
    while (i < 0)
        i+=cups->n;
    cups->n++;
    i = i%cups->n;
    memmove(cups->cups+i+1, cups->cups+i, (cups->n-i-1) * sizeof(*cups->cups));
    cups->indices[cup] = i;
    cups->cups[i] = cup;
    //for (int j = 0; j < cups->max+1; j++)
        //if (cups->indices[j] > i)
            //cups->indices[j]++;
}

int rem(Cups *cups, int i)
{
    //printf("Removing %d\n", i);
    //printCups(cups);
    while (i < 0)
        i+=cups->n;
    i = i%cups->n;
    int res = cups->cups[i];
    memmove(cups->cups+i, cups->cups+i+1, (cups->n-i-1) * sizeof(*cups->cups));
    cups->n--;
    //for (int j = 0; j < cups->max+1; j++)
        //if (cups->indices[j] > i)
            //cups->indices[j]--;
    //printf("----> ");
    //printCups(cups);
    return res;
}

void move(Cups *cups, int from, int to)
{
    from = from % cups->n;
    to = to % cups->n;
    printf("Moving from %d to %d\n", from, to);
    if (from == to)
        return;
    int val = cups->cups[from];
    if (from < to)
        // TODO. Mismatch
        memmove(cups->cups+from, cups->cups+from+1, (to-from) * sizeof(*cups->cups));
    else
        memmove(cups->cups+to+1, cups->cups+to, (from-to) * sizeof(*cups->cups));
    cups->cups[to] = val;
    // TODO: Indices
}

void set(Cups *cups, int i, int cup)
{
    while (i < 0)
        i+=cups->n;
    cups->cups[i%cups->n] = cup;
    cups->indices[cup] = i%cups->n;
}

int get(Cups *cups, int i)
{
    while(i < 0)
        i+=cups->n;
    return cups->cups[i%cups->n];
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

