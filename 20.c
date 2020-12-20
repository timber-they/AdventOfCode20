#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;

#define SIZE 10
#define IMG_SIZE 12
#define TILE_CNT (IMG_SIZE * IMG_SIZE)

typedef struct Tile{
    long id;
    char image[SIZE*SIZE];
    int swapXY;
    int flipX;
    int flipY;
    int oriented;
} Tile;

int matches(Tile a, Tile b);
Tile *parse(FILE *in);
void parseLine(char *line);
int getMatchingCount(Tile *all, int one);
char *constructImage(Tile *tiles);
Tile *getCorners(Tile *tiles);
Tile *constructTiles(Tile *tiles, Tile *corners);
void assignOrientationCorner(Tile corner, Tile *tiles);
Tile getMatchingLeft(Tile *tiles, char *border);
int matchesBorder(Tile a, char *line);

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
    Tile *corners = getCorners(tiles);
    Tile *constructed = constructTiles(tiles, corners);
    return NULL;
}

Tile getMatchingLeft(Tile *tiles, char *border)
{
    /*for (int i = 0; i < TILE_CNT; i++)
        if (tiles[i].id != toMatch.id)
        {
            int m = matches(tiles[i], toMatch);
            if (m)
            {
                // TBLR

            }
        }*/
    Tile res = {0};
    return res;
}

Tile *constructTiles(Tile *tiles, Tile *corners)
{
    // Choose a random corner, as we worry about orientation later
    Tile start = *corners;
    Tile *res = malloc(TILE_CNT * sizeof(*res));
    for (int y = 0; y < IMG_SIZE; y++)
        for (int x = 0; x < IMG_SIZE; x++)
        {
            if (y == 0)
            {
                if (x == 0)
                {
                    // First corner
                    assignOrientationCorner(start, tiles);
                    continue;
                }
                if (x == IMG_SIZE - 1)
                {
                    // Second corner
                    continue;
                }
                // First row
                continue;
            }
            if (y == IMG_SIZE - 1)
            {
                if (x == 0)
                {
                    // Third order
                    continue;
                }
                if (x == IMG_SIZE - 1)
                {
                    // Last order
                    continue;
                }
                // Last row
                continue;
            }
            if (x == 0)
            {
                // Fist col
                continue;
            }
            if (x == IMG_SIZE - 1)
            {
                // Last col
                continue;
            }
            // Something in the middle
        }
    return NULL;
}

// Orientates the corner as top left
void assignOrientationCorner(Tile corner, Tile *tiles)
{
    for (int j = 0; j < TILE_CNT; j++)
    {
        if (corner.id != tiles[j].id)
        {
            int m = matches(corner, tiles[j]);
            if (m > 0)
            {
                m--;
                switch(m%4)
                {
                    case 0:
                        corner.flipY = 1;
                        // Top
                        break;
                    case 1:
                        // Bottom
                        break;
                    case 2:
                        // Left
                        corner.flipX = 1;
                        break;
                    case 3:
                        // Right
                        break;
                }
            }
        }
    }
    corner.oriented = 1;
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
            res += m > 0;
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
    // Reverse
    for (i = 0; i < SIZE; i++)
        if (a[i] != b[SIZE-i-1])
            break;
    if (i == SIZE)
        return 2;
    return 0;
}
char **getBorders(char *arr)
{
    char **borders = malloc(4 * sizeof(*borders));
    // Upper border
    borders[0] = malloc(SIZE * sizeof(*borders[0]));
    for (int i = 0; i < SIZE; i++)
        borders[0][i] = arr[i];
    // Bottom border
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

// Returns which border+1 (UBLR) + 1 iff flipped matched, or 0 otherwise
int matches(Tile a, Tile b)
{
    char **bordersB = getBorders(b.image);
    int res = 0;
    for (int j = 0; j < 4; j++)
    {
        res = matchesBorder(a, bordersB[j]);
        if (res)
            break;
    }
    for (int i = 0; i < 4; i++)
        free(bordersB[i]);
    free(bordersB);
    return res;
}
int matchesBorder(Tile a, char *border)
{
    char **bordersA = getBorders(a.image);
    int res = 0;
    for (int i = 0; i < 4; i++)
    {
        int m = matchLine(bordersA[i], border);
        if (m)
        {
            res = i+1 + 4*(m-1);
            goto end;
        }
    }
end:
    for (int i = 0; i < 4; i++)
        free(bordersA[i]);
    free(bordersA);
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

