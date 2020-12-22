#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARD_COUNT 50

size_t iterations = 0;

typedef struct Queue {
    int start;
    int length;
    int data[CARD_COUNT];
} Queue;

int dequeue(Queue *queue);
void enqueue(Queue *queue, int val);
void play(Queue *a, Queue *b);
int score(Queue *q);
Queue *getQueues(FILE *in);
int part1(FILE *in);
void printQueue(Queue *queue);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in22", "r");

    printf("Part 1: %d\n", part1(in));

	fclose(in);	
	return 0;	
}

int part1(FILE *in)
{
    Queue *queues = getQueues(in);
    play(queues, queues+1);
    int res = score(queues[0].length > 0 ? queues : queues+1);
    free(queues);
    return res;
}

Queue *getQueues(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    Queue *res = calloc(2, sizeof(*res));
    while (getline(&line, &n, in) > 0 && *line != '\n')
    {
        if (strstr(line, "Player") != NULL)
            continue;
        *strchr(line, '\n') = '\0';
        enqueue(res, atoi(line));
    }
    while (getline(&line, &n, in) > 0 && *line != '\n')
    {
        if (strstr(line, "Player") != NULL)
            continue;
        *strchr(line, '\n') = '\0';
        enqueue(res+1, atoi(line));
    }
    free(line);
    return res;
}

void play(Queue *a, Queue *b)
{
    while (a->length != 0 && b->length != 0)
    {
        int aTop = dequeue(a);
        int bTop = dequeue(b);
        if (aTop > bTop)
        {
            enqueue(a, aTop);
            enqueue(a, bTop);
        }
        else
        {
            enqueue(b, bTop);
            enqueue(b, aTop);
        }
    }
}

int score(Queue *q)
{
    int res = 0;
    for (int i = 0; i < q->length; i++)
        res += (i+1) * q->data[(q->start + i) % CARD_COUNT];
    return res;
}

int dequeue(Queue *queue)
{
    queue->length--;
    if (queue->length < 0)
    {
        return -1;
    }
    return queue->data[(queue->start+queue->length) % CARD_COUNT];
}

void enqueue(Queue *queue, int val)
{
    queue->length++;
    if (queue->length > CARD_COUNT)
    {
        fprintf(stderr, "Index out of rage\n");
        return;
    }
    queue->start--;
    if (queue->start < 0)
        queue->start += CARD_COUNT;
    queue->data[queue->start] = val;
}

void printQueue(Queue *queue)
{
    for (int i = 0; i < queue->length; i++)
        printf("%d, ", queue->data[(queue->start+i)%CARD_COUNT]);
    printf("\n");
}

