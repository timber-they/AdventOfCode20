#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CARD_COUNT 50
#define MAX_ROUNDS 10000

size_t iterations = 0;

typedef struct Queue {
    int start;
    int length;
    int data[CARD_COUNT];
} Queue;

typedef struct Game{
    Queue player1;
    Queue player2;
    Queue *record1[MAX_ROUNDS];
    Queue *record2[MAX_ROUNDS];
    int playedRounds;
} Game;

int dequeue(Queue *queue);
void enqueue(Queue *queue, int val);
void play(Queue *a, Queue *b);
int score(Queue *q);
Queue *getQueues(FILE *in);
int part1(FILE *in);
void printQueue(Queue *queue);
int playRecursive(Game *game);
int isReplay(Game *game);
int equal(Queue *a, Queue *b);
int get(Queue *q, int i);
Game *recurse(Game *game, int a, int b);
Queue yeet(Queue queue, int count);
void reg(Game *game);
Game *getGame(FILE *in);
int part2(FILE *in);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in22", "r");

    printf("Part 1: %d\n", part1(in));
    rewind(in);
    printf("Part2: %d\n", part2(in));

	fclose(in);	
	return 0;	
}

int part2(FILE *in)
{
    Game *game = getGame(in);
    int won = playRecursive(game);
    //printf("Game over:\n");
    //printQueue(&(game->player1));
    //printQueue(&(game->player2));
    int res = score(won == 1 ? &(game->player1) : &(game->player2));
    for (int i = 0; i <= game->playedRounds; i++)
    {
        free(game->record1[i]);
        free(game->record2[i]);
    }
    free(game);
    return res;
}

int equal(Queue *a, Queue *b)
{
    if (a->length != b->length)
        return 0;
    for (int i = 0; i < a->length; i++)
        if (get(a, i) != get(b, i))
            return 0;
    return 1;
}

int isReplay(Game *game)
{
    for (int i = 0; i < game->playedRounds; i++)
    {
        if (game->record1[i] == NULL)
        {
            fprintf(stderr, "That was unexpected\n");
            break;
        }
        if (equal(&(game->player1), game->record1[i]) ||
            equal(&(game->player2), game->record2[i]))
            return 1;
    }
    return 0;
}

int currentRecursion = 0;
int playRecursive(Game *game)
{
    currentRecursion++;
    int round = 0;
    while (game->player1.length != 0 && game->player2.length != 0)
    {
        round++;
        //printf("Game %d, round %d:\n", currentRecursion, round);
        //printQueue(&(game->player1));
        //printQueue(&(game->player2));
        if (isReplay(game))
        {
            //printf("Replay detected!\n");
            // Player 1 won
            return 1;
        }
        reg(game);
        int top1 = dequeue(&(game->player1));
        int top2 = dequeue(&(game->player2));
        int winner = -1;
        if (top1 > game->player1.length || top2 > game->player2.length)
            winner = (top2 > top1) + 1;
        else
        {
            //printf("Entering recursion (%d <= %d && %d <= %d)\n==================\n", top1, game->player1.length, top2, game->player2.length);
            Game *sub  = recurse(game, top1, top2);
            winner = playRecursive(sub);
            currentRecursion--;
            for (int i = 0; i <= sub->playedRounds; i++)
            {
                free(sub->record1[i]);
                free(sub->record2[i]);
            }
            free(sub);
        }
        //printf("Player %d wins the round!\n\n", winner);
        if (winner == 1)
        {
            enqueue(&(game->player1), top1);
            enqueue(&(game->player1), top2);
        }
        else
        {
            enqueue(&(game->player2), top2);
            enqueue(&(game->player2), top1);
        }
    }

    return (game->player1.length > 0 ? 1 : 2);
}

void reg(Game *game)
{
    Queue *clone1 = malloc(sizeof(*clone1));
    memcpy(clone1, &(game->player1), sizeof(*clone1));
    Queue *clone2 = malloc(sizeof(*clone2));
    memcpy(clone2, &(game->player2), sizeof(*clone2));
    game->record1[game->playedRounds] = clone1;
    game->record2[game->playedRounds] = clone2;
    game->playedRounds++;
    if (game->playedRounds >= MAX_ROUNDS)
        fprintf(stderr, "Too many rounds\n");
}

Game *recurse(Game *game, int a, int b)
{
    Game *res = calloc(1, sizeof(*res));
    res->player1 = yeet(game->player1, a);
    res->player2 = yeet(game->player2, b);
    return res;
} 

Queue yeet(Queue queue, int count)
{
    Queue res = {0};
    for (int i = count-1; i >= 0; i--)
        enqueue(&res, get(&queue, queue.length - (count - i)));
    return res;
}

Game *getGame(FILE *in)
{
    Queue *queues = getQueues(in);
    Game *res = calloc(1, sizeof(*res));
    res->player1 = queues[0];
    res->player2 = queues[1];
    free(queues);
    return res;
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
        res += (i+1) * get(q, i);
    return res;
}

int get(Queue *q, int i)
{
    return q->data[(q->start+i) % CARD_COUNT];
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
    for (int i = queue->length-1; i >= 0; i--)
        printf("%d, ", queue->data[(queue->start+i)%CARD_COUNT]);
    printf("\n");
}

