#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_COUNT 9

typedef struct Cups{
    int n;
    char *cups;
} Cups;

size_t iterations = 0;
// The cups are labelled with ASCII characters '0' - '9'
Cups *getCups(FILE *in);
// Start at 0
void playRound(Cups *cups, int current);
char get(Cups *cups, int i);
void set(Cups *cups, int i, char cup);
char *getP(Cups *cups, int i);
void insert(Cups *cups, int i, char cup);
// Returns the removed cup
char rem(Cups *cups, int i);
int getDestination(Cups *cups, int current, char destValue);
void play(Cups *cups, int rounds);
void printCups(Cups *cups);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in23", "r");

    Cups *cups = getCups(in);
    play(cups, 100);
    printCups(cups);

    free(cups->cups);
    free(cups);
	fclose(in);	
	return 0;	
}

void printCups(Cups *cups)
{
    int i;
    for (i = 0; i < cups->n && get(cups, i) != '1'; i++);
    if (get(cups, i) != '1')
    {
        fprintf(stderr, "Couldn't find starting cup\n");
        exit(1);
    }
    for (int j = 1; j < cups->n; j++)
        printf("%c", get(cups, j+i));
    printf("\n");
}

void play(Cups *cups, int rounds)
{
    for (int i = 0; i < rounds; i++)
        playRound(cups, i);
}

void playRound(Cups *cups, int current)
{
    char c = get(cups, current);
    char r1 = rem(cups, current+1);
    char r2 = rem(cups, current+1);
    char r3 = rem(cups, current+1);
    int destination = getDestination(cups, current, c-1);
    insert(cups, destination+1, r3);
    insert(cups, destination+1, r2);
    insert(cups, destination+1, r1);
    printf("Played round...\n");
}

int getDestination(Cups *cups, int current, char destValue)
{
    if (destValue < '1')
        destValue = '9';
    printf("Getting destination of %c with current %d (count %d)\n", destValue, current, cups->n);
    for (int i = (current+1)%cups->n; i != current%cups->n; i = (i+1)%cups->n)
        if (get(cups, i) == destValue)
            return i;
    return getDestination(cups, current, destValue-1);
}

void insert(Cups *cups, int i, char cup)
{
    printf("Inserting %c at %d\n", cup, i);
    while (i < 0)
        i+=cups->n;
    i = i%cups->n;
    if (i == 0)
    {
        cups->n++;
        set(cups, cups->n-1, cup);
        return;
    }
    char buff = get(cups, i);
    set(cups, i, cup);
    insert(cups, i+1, buff);
}

char rem(Cups *cups, int i)
{
    while (i < 0)
        i+=cups->n;
    char res = get(cups, i);
    for (int j = 1; i+j < cups->n; j++)
        set(cups, i+j-1, get(cups, i+j));
    cups->n--;
    return res;
}

void set(Cups *cups, int i, char cup)
{
    while (i < 0)
        i+=cups->n;
}

char get(Cups *cups, int i)
{
    while(i < 0)
        i+=cups->n;
    return cups->cups[i%cups->n];
}

char *getP(Cups *cups, int i)
{
    while(i < 0)
        i+=cups->n;
    return cups->cups+(i%cups->n);
}

Cups *getCups(FILE *in)
{
    char *res = NULL;
    size_t n = 0;
    if (getline(&res, &n, in) <= 0 || *res == '\n' || *res == '\0')
    {
        fprintf(stderr, "Invalid input\n");
        free(res);
        exit(1);
    }
    Cups *cups = calloc(1, sizeof(*cups));
    cups->cups = res;
    cups->n = CUP_COUNT;
    return cups;
}

