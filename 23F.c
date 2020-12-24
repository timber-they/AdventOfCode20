#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUP_COUNT 9
#define CUP_COUNT2 1000000
#define ROUNDS 100
#define ROUNDS2 10000000

typedef struct Cup{
    int label;
    int max;
    struct Cup *clockwise;
    struct Cup *decLabel;
} Cup;

Cup *read(FILE *in, int cupCount);
void play(Cup *cups, int rounds);
Cup *playRound(Cup *current);
void printCups(Cup *cups);
void print2(Cup *cups);
void freeCups(Cup *cups);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in23", "r");

    Cup *cups = read(in, CUP_COUNT);
    play(cups, ROUNDS);
    printCups(cups);

    freeCups(cups);
    rewind(in);
    cups = read(in, CUP_COUNT2);
    play(cups, ROUNDS2);
    print2(cups);

    freeCups(cups);
	fclose(in);	
	return 0;	
}

void play(Cup *cups, int rounds)
{
    for (int i = 1; i <= rounds; i++)
    {
        //printf("========\nRound %2d\n========\n", i);
        cups = playRound(cups);
    }
}

void printCups(Cup *cups)
{
    while (cups->label != 1)
        cups = cups->clockwise;
    for (cups = cups->clockwise; cups->label != 1; cups = cups->clockwise)
        printf("%d", cups->label);
    printf("\n");
}

void print2(Cup *cups)
{
    while (cups->label != 1)
        cups = cups->clockwise;
    int next = cups->clockwise->label;
    int next2 = cups->clockwise->clockwise->label;
    printf("%d * %d = %ld\n", next, next2, ((long)next) * ((long)next2));
}

void freeCups(Cup *cups)
{
    while (cups->label != 1)
        cups = cups->clockwise;
    for (cups = cups->clockwise; cups->label != 1;)
    {
        Cup *curr = cups->clockwise;
        free(cups);
        cups = curr;
    }
    free(cups);
}

Cup *playRound(Cup *current)
{
    Cup *n1 = current->clockwise;
    Cup *n2 = n1->clockwise;
    Cup *n3 = n2->clockwise;
    Cup *dest = current->decLabel;
    while (dest == n1 || dest == n2 || dest == n3)
        dest = dest->decLabel;
    //printf("%d (%d, %d, %d) -> %d\n", current->label, n1->label, n2->label, n3->label, dest->label);
    //printCups(current);
    current->clockwise = n3->clockwise;
    n3->clockwise = dest->clockwise;
    dest->clockwise = n1;
    return current->clockwise;
}

Cup *read(FILE *in, int cupCount)
{
    //printf("Reading %d\n", cupCount);
    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, in) <= 0 || *line == '\n' || *line == '\0')
    {
        fprintf(stderr, "Invalid input\n");
        exit(1);
    }

    Cup *res = calloc(1, sizeof(*res));
    Cup *curr = res;
    Cup *maxLast = NULL;
    for (int i = 0; i < CUP_COUNT; i++)
    {
        curr->max = cupCount;
        curr->label = line[i] - '0';
        //printf("Got %d\n", curr->label);
        if (curr->label == CUP_COUNT)
            maxLast = curr;
        if (i+1 < cupCount)
        {
            curr->clockwise = calloc(1, sizeof(*curr->clockwise));
            curr = curr->clockwise;
        }
    }
    for (int i = CUP_COUNT+1; i <= cupCount; i++)
    {
        curr->max = cupCount;
        curr->label = i;
        curr->decLabel = maxLast;
        maxLast = curr;

        if (i < cupCount)
        {
            curr->clockwise = calloc(1, sizeof(*curr->clockwise));
            curr = curr->clockwise;
        }
    }
    Cup *a;
    for (a = res; a != NULL && a->decLabel == NULL; a = a->clockwise)
    {
        //printf("a->label = %d\n", a->label);
        Cup *b;
        for (b = res; b != NULL; b = b->clockwise)
            if (b->label == a->label - 1)
            {
                a->decLabel = b;
                break;
            }
        // No dec found - start at max
        if (b == NULL)
            for (b = res; b != NULL; b = b->clockwise)
                if (b->label == cupCount)
                {
                    a->decLabel = b;
                    break;
                }
    }
    //printf("Attaching %d to end of %d\n", res->label, curr->label);
    curr->clockwise = res;
    free (line);
    return res;
}

