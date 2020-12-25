#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int transform(int subject, int loopSize);
int getLoopSize(int publicKey);
int getEncryptionKey(int cardKey, int doorKey);
int part1(FILE *in);
int part2(FILE *in);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in25", "r");

    printf("Part1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d lol\n", part2(in));

	fclose(in);	
	return 0;	
}

int part2(FILE *in)
{
    return 42;
}

int part1(FILE *in)
{
    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, in) <= 0)
    {
        fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    int cardKey = atoi(line);
    if (getline(&line, &n, in) <= 0)
    {
        fprintf(stderr, "Invalid input\n");
        exit(1);
    }
    int doorKey = atoi(line);
    free(line);
    return getEncryptionKey(cardKey, doorKey);
}

int getEncryptionKey(int cardKey, int doorKey)
{
    return transform(doorKey, getLoopSize(cardKey));
}

int transform(int subject, int loopSize)
{
    long res = 1;
    for (int i = 0; i < loopSize; i++)
        res = (res * subject) % 20201227;
    return (int) res;
}

int getLoopSize(int publicKey)
{
    int current = 1;
    for (int loop = 0;; loop++)
    {
        if (current == publicKey)
        {
            printf("Loop size: %d\n", loop);
            return loop;
        }
        current = (current * 7) % 20201227;
    }
    fprintf(stderr, "Hang on, what?!\n");
    exit(1);
}

