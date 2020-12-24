#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TILES 592
// A guess.
#define MAX_TILES 10000
#define ITERATION 100

/*
 *  - 0 1 2 3 4 5
 * - 0 1 2 3 4 5
 *  0 1 2 3 4 5
 */
typedef struct Coord{
    int x;
    int y;
    // 0 -> not registered, 1 -> flipped, 2 -> unflipped
    int flipState;
} Coord;

Coord *getCoords(FILE *in);
Coord parseLine(char *line);
Coord *iterate(Coord *current);
int countFlipped(Coord *coords);
int areAdjacent(Coord a, Coord b);
int minX(Coord *coords);
int minY(Coord *coords);
int maxX(Coord *coords);
int maxY(Coord *coords);
int isFlipped(Coord *coords, Coord coord);
int getAdjacent(Coord *coords, Coord coord);
void printCoords(Coord *coords, Coord highlight);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in24", "r");

    Coord *coords = getCoords(in);
    printf("Flipped to black: %d\n", countFlipped(coords));

    for (int i = 0; i < 100; i++)
        coords = iterate(coords);
    printf("Finally flipped to blacK: %d\n", countFlipped(coords));

    free(coords);
	fclose(in);	
	return 0;	
}

void printCoords(Coord *coords, Coord highlight)
{
    int miX = minX(coords);
    int maX = maxX(coords);
    int miY = minY(coords);
    int maY = maxY(coords);
    int *array = calloc((maX-miX+1)*(maY-miY+1), sizeof(*array));
    for (int i = 0; coords[i].flipState; i++)
        if (coords[i].flipState == 1)
        {
            array[coords[i].x - miX + (coords[i].y - miY) * (maX-miX+1)] = 
                coords[i].x == highlight.x && coords[i].y == highlight.y
                ? 2
                : 1;
        }
    if (highlight.x >= miX && highlight.y >= miY && 
        highlight.x <= maX && highlight.y <= maY &&
            !array[highlight.x - miX + (highlight.y - miY) * (maX-miX+1)])
        array[highlight.x - miX + (highlight.y - miY) * (maX-miX+1)] = -1;
    for (int y = maY-miY; y >= 0; y--)
    {
        printf("%2d ", y);
        for (int i = 0; i < y; i++)
            printf(" ");
        for (int x = 0; x <= maX-miX; x++)
        {
            switch(array[x + y * (maX-miX+1)])
            {
                case 1:
                    printf("%d ", x);
                    break;
                case 2:
                    printf("█ ");
                    break;
                case -1:
                    printf("_ ");
                    break;
                default:
                    printf(". ");
                    break;
            }
        }
        printf("\n");
    }
    free(array);
}

Coord *iterate(Coord *current)
{
    //Coord highlight = {0};
    //highlight.x = 1000;
    //highlight.y = 1000;
    //printCoords(current, highlight);
    //printf("\n=================\n");
    Coord *clone = malloc(MAX_TILES * sizeof(*clone));
    int cloneIndex = 0;
    memcpy(clone, current, MAX_TILES * sizeof(*clone));
    Coord coord = {0}; 
    int miX = minX(current);
    int maX = maxX(current);
    int miY = minY(current);
    int maY = maxY(current);
    for (coord.x = miX-1; coord.x <= maX+1; coord.x++)
        for (coord.y = miY-1; coord.y <= maY+1; coord.y++)
        {
            int flipped = isFlipped(current, coord);
            // We only register new flipped ones
            coord.flipState = 1;
            int adjacent = getAdjacent(current, coord);
            if (flipped && (adjacent == 1 || adjacent == 2))
            {
                // Stays black
                //printf("(%d,%d) has %d adjacents - stays flipped\n", coord.x, coord.y, adjacent);
                //printCoords(current, coord);
                clone[cloneIndex++] = coord;
            }
            else if (!flipped && adjacent == 2)
            {
                // Flips
                //printf("(%d,%d) has %d adjacents - flips\n", coord.x, coord.y, adjacent);
                //printCoords(current, coord);
                clone[cloneIndex++] = coord;
            }
        }
    free(current);
    for (; cloneIndex < MAX_TILES; cloneIndex++)
        clone[cloneIndex].flipState = 0;
    //printf("Currently flipped: %d (cloneIndex: %d)\n", countFlipped(clone), cloneIndex);
    return clone;
}

int getAdjacent(Coord *coords, Coord coord)
{
    int res = 0;
    for (int i = 0; coords[i].flipState; i++)
        if (areAdjacent(coords[i], coord))
        {
            if (coords[i].flipState == 1)
                // TODO: Something's wrong
                //printf("(%d,%d) adjacent to (%d,%d)\n", coords[i].x, coords[i].y, coord.x, coord.y);
            res += (coords[i].flipState == 1);
        }
    return res;
}

int isFlipped(Coord *coords, Coord coord)
{
    for (int i = 0; coords[i].flipState; i++)
        if (coords[i].x == coord.x && coords[i].y == coord.y)
            return coords[i].flipState == 1;
    return 0;
}

int minX(Coord *coords)
{
    int min = coords[0].x;
    for (int i = 1; coords[i].flipState; i++)
        if (coords[i].x < min)
            min = coords[i].x;
    return min;
}
int minY(Coord *coords)
{
    int min = coords[0].y;
    for (int i = 1; coords[i].flipState; i++)
        if (coords[i].y < min)
            min = coords[i].y;
    return min;
}
int maxX(Coord *coords)
{
    int max = coords[0].x;
    for (int i = 1; coords[i].flipState; i++)
        if (coords[i].x > max)
            max = coords[i].x;
    return max;
}
int maxY(Coord *coords)
{
    int max = coords[0].y;
    for (int i = 1; coords[i].flipState; i++)
        if (coords[i].y > max)
            max = coords[i].y;
    return max;
}

int areAdjacent(Coord a, Coord b)
{
    return (a.y == b.y && (a.x == b.x-1 || a.x == b.x+1)) ||
           (a.y == b.y+1 && (a.x == b.x-1 || a.x == b.x)) ||
           (a.y == b.y-1 && (a.x == b.x || a.x == b.x+1));
}

int countFlipped(Coord *coords)
{
    int res = 0;
    for (int i = 0; i < MAX_TILES; i++)
    {
        res += coords[i].flipState == 1;
        //if (coords[i].flipState == 1)
            //printf("(%d,%d) ", coords[i].x, coords[i].y);
    }
    return res;
}

Coord *getCoords(FILE *in)
{
    Coord *res = calloc(MAX_TILES, sizeof(*res));
    size_t n = 0;
    char *line = NULL;
    while (getline(&line, &n, in) >= 0 && *line != '\n')
    {
        *strchr(line, '\n') = '\0';
        Coord parsed = parseLine(line);
        for (int i = 0; i < TILES; i++)
        {
            if (res[i].flipState && res[i].x == parsed.x && res[i].y == parsed.y)
            {
                res[i].flipState = res[i].flipState == 1 ? 2 : 1;
                break;
            }
            if (!res[i].flipState)
            {
                res[i] = parsed;
                res[i].flipState = 1;
                break;
            }
        }
    }
    free(line);
    return res;
}

Coord parseLine(char *line)
{
    Coord res = {0};
    for (int i = 0; i < strlen(line); i++)
    {
        switch(line[i])
        {
            case 'e':
                res.x++;
                break;
            case 'w':
                res.x--;
                break;
            case 'n':
                res.y++;
                i++;
                switch(line[i])
                {
                    case 'e':
                        break;
                    case 'w':
                        res.x--;
                        break;
                    default:
                        fprintf(stderr, "Unexpected follow up to n: %c\n", line[i]);
                        exit(1);
                }
                break;
            case 's':
                res.y--;
                i++;
                switch(line[i])
                {
                    case 'e':
                        res.x++;
                        break;
                    case 'w':
                        break;
                    default:
                        fprintf(stderr, "Unexpected follow up to s: %c\n", line[i]);
                        exit(1);
                }
                break;
            default:
                fprintf(stderr, "Unexpected: %c (%s)\n", line[i], line);
                exit(1);
        }
    }
    return res;
}

