#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_COUNT 9
#define CUP_COUNT2 1000000

typedef struct Cups{
    int n;
    int max;
    int *cups;
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
int getDestination(Cups *cups, int current, int destValue);
void play(Cups *cups, int rounds);
void printCups(Cups *cups);
void print2(Cups *cups);
int getIndex(Cups *cups, int cup);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in23", "r");

    Cups *cups = getCups(in);
    play(cups, 100);
    printCups(cups);

    rewind(in);
    free(cups->cups);
    free(cups);

    cups = getCups2(in);
    play(cups, 10000000);
    print2(cups);

    free(cups->cups);
    free(cups);
	fclose(in);	
	return 0;	
}

void printCups(Cups *cups)
{
    int i;
    for (i = 0; i < cups->n && get(cups, i) != 1; i++);
    if (get(cups, i) != 1)
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
    int i;
    for (i = 0; i < cups->n && get(cups, i) != 1; i++);
    if (get(cups, i) != 1)
        printf("Couldn't find starting cup");
    else
    {
        int a = get(cups, i+1), b = get(cups, i+2);
        printf("%d * %d = %d", a, b, a*b);
    }
}

void play(Cups *cups, int rounds)
{
    int last = 0;
    int nextPrint = 1;
    for (int i = 0; i < rounds; i++)
    {
        if (i >= nextPrint)
        {
            printf("Round %d\n", i);
            nextPrint *= 10;
        }
        //printf("=================\nRound %d (last %d)\n=================\n", i+1, last);
        //printCups(cups);
        last = playRound(cups, last);
    }
}

int getIndex(Cups *cups, int cup)
{
    int i;
    for (i = 0; i < cups->n; i++)
        if (get(cups, i) == cup)
            break;
    //printf("Looking for index of %c yielded %d\n", cup, i);
    return i;
} 

int playRound(Cups *cups, int last)
{
    int current = last == 0 ? 0 : (getIndex(cups, last) + 1) % cups->n;
    int c = get(cups, current);
    //printf("c=%d\n", c);
    int r1 = rem(cups, current+1);
    current = getIndex(cups, c);
    int r2 = rem(cups, current+1);
    current = getIndex(cups, c);
    int r3 = rem(cups, current+1);
    current = getIndex(cups, c);
    int destination = getDestination(cups, current, c-1);
    //printf("Current: %d (%d); (%d, %d, %d); destination: %d (%d)\n", c, current, r1, r2, r3, get(cups, destination), destination);
    //printCups(cups);
    insert(cups, destination+1, r3);
    insert(cups, destination+1, r2);
    insert(cups, destination+1, r1);
    //printf("Played round...\n");
    return c;
}

int getDestination(Cups *cups, int current, int destValue)
{
    if (destValue < 1)
        destValue = cups->max;
    //printf("Getting destination of %d with current %d (count %d)\n", destValue, current, cups->n);
    for (int i = (current+1)%cups->n; i != current%cups->n; i = (i+1)%cups->n)
        if (get(cups, i) == destValue)
            return i;
    return getDestination(cups, current, destValue-1);
}

void insert(Cups *cups, int i, int cup)
{
    //printf("Inserting %d at %d\n", cup, i);
    while (i < 0)
        i+=cups->n;
    cups->n++;
    i = i%cups->n;
    memmove(cups->cups+i+1, cups->cups+i, (cups->n-i-1) * sizeof(*cups->cups));
    cups->cups[i] = cup;
}

int rem(Cups *cups, int i)
{
    //printf("Removing %d\n", i);
    //printCups(cups);
    while (i < 0)
        i+=cups->n;
    i = i%cups->n;
    int res = get(cups, i);
    memmove(cups->cups+i, cups->cups+i+1, (cups->n-i-1) * sizeof(*cups->cups));
    cups->n--;
    //printf("----> ");
    //printCups(cups);
    return res;
}

void set(Cups *cups, int i, int cup)
{
    while (i < 0)
        i+=cups->n;
    cups->cups[i%cups->n] = cup;
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
        res[i] = line[i] - '0';
    free(line);
    Cups *cups = calloc(1, sizeof(*cups));
    cups->cups = res;
    cups->n = CUP_COUNT;
    cups->max = CUP_COUNT;
    //printf("Cup count: %d\n", cups->n);
    return cups;
}

Cups *getCups2(FILE *in)
{
    int *res = calloc(CUP_COUNT2, sizeof(*res));
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
        res[i] = line[i] - '0';
    for (int i = CUP_COUNT; i < CUP_COUNT2; i++)
        res[i] = i+1;
    free(line);
    Cups *cups = calloc(1, sizeof(*cups));
    cups->cups = res;
    cups->n = CUP_COUNT2;
    cups->max = CUP_COUNT2;
    //printf("Cup count: %d\n", cups->n);
    return cups;
}

