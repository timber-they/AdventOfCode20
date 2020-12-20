#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;

#define SIZE 10
#define TILE_CNT 144

typedef struct Tile{
    long id;
    char image[SIZE*SIZE];
} Tile;

int matches(Tile a, Tile b);
Tile *parse(FILE *in);
void parseLine(char *line);
int getMatchingCount(Tile *all, int one);
char *constructImage(Tile *tiles);
Tile *getCorners(Tile *tiles);

int main()
{
	FILE *in = fopen("in20", "r");

    Tile *parsed = parse(in);
    long part1 = 1;
    Tile *corners = getCorners(parsed);
    for (int i = 0; i < 4; i++)
        part1 *= corners[i].id;
    printf("Part 1: %ld\n", part1);

    free(parsed);
    free(corners);
	fclose(in);	
	return 0;	
}

char *constructImage(Tile *tiles)
{
    return NULL;
}

Tile *getCorners(Tile *tiles)
{
    int cornerCount = 0;
    Tile *res = malloc(4 * sizeof(*res));
    for (int i = 0; i < TILE_CNT; i++)
        if (getMatchingCount(tiles, i) == 2)
            res[cornerCount++] = tiles[i];
    return res;
}

int getMatchingCount(Tile *all, int one)
{
    int res = 0;
    for (int i = 0; i < TILE_CNT; i++)
        if (one != i)
        {
            int m = matches(all[one], all[i]);
            res += m;
        }
    return res;
}

int matchLine (char *a, char *b)
{
    // In order
    int i;
    for (i = 0; i < SIZE; i++)
        if (a[i] != b[i])
            break;
    if(i == SIZE)
        return 1;
    for (i = 0; i < SIZE; i++)
        if (a[i] != b[SIZE-i-1])
            break;
    if (i == SIZE)
        return 1;
    return 0;
}
char **getBorders(char *arr)
{
    char **borders = malloc(4 * sizeof(*borders));
    // Upper border
    borders[0] = malloc(SIZE * sizeof(*borders[0]));
    for (int i = 0; i < SIZE; i++)
        borders[0][i] = arr[i];
    // Lower border
    borders[1] = malloc(SIZE * sizeof(*borders[1]));
    for (int i = 0; i < SIZE; i++)
        borders[1][i] = arr[(SIZE-1) * SIZE + i];
    // Left border
    borders[2] = malloc(SIZE * sizeof(*borders[2]));
    for (int i = 0; i < SIZE; i++)
        borders[2][i] = arr[SIZE * i];
    // Right border
    borders[3] = malloc(SIZE * sizeof(*borders[3]));
    for (int i = 0; i < SIZE; i++)
        borders[3][i] = arr[SIZE * i + SIZE - 1];
    return borders;
}
int matches(Tile a, Tile b)
{
    char **bordersA = getBorders(a.image);
    char **bordersB = getBorders(b.image);
    int res = 0;
    for (int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if (matchLine(bordersA[i], bordersB[j]))
            {
                res = 1;
                goto end;
            }
end:
    for (int i = 0; i < 4; i++)
    {
        free(bordersA[i]);
        free(bordersB[i]);
    }
    free(bordersA);
    free(bordersB);
    return res;
}

Tile *parse(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    Tile *res = calloc(TILE_CNT, sizeof(*res));
    Tile currentTile = {0};
    int currentTileLine = 0;
    int resIndex = 0;
    while (getline(&line, &n, in) > 0 && *line != '\0')
    {
        if (*line == '\n' && currentTileLine == 0)
           // Done
           break; 
        if (*line == '\n')
        {
            // Tile done
            res[resIndex++] = currentTile;
            memset(&currentTile, 0, sizeof(currentTile));
            currentTileLine = 0;
            continue;
        }
        if (strstr(line, "Tile") != NULL)
        {
            // Header
            *strchr(line, ':') = '\0';
            currentTile.id = atol(strchr(line, ' ')+1);
            continue;
        }

        parseLine(line);
        for (int i = 0; i < SIZE; i++)
            currentTile.image[currentTileLine * SIZE + i] = line[i];
        currentTileLine++;
    }

    free(line);
    return res;
}

void parseLine(char *line)
{
    *strchr(line, '\n') = '\0';
    char *ptr;
    while ((ptr = strrchr(line, '#')) != NULL)
        *ptr = 1;
    while ((ptr = strrchr(line, '.')) != NULL)
        *ptr = 0;
}

