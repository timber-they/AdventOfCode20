#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;

#define SIZE 10
#define CROP_SIZE (SIZE - 2)
#define IMG_SIZE 12
#define TILE_CNT (IMG_SIZE * IMG_SIZE)
#define CON_SIZE (CROP_SIZE * IMG_SIZE)
#define MONSTER_LENGTH 20
#define MONSTER_HEIGHT 3

#define INDEX(x,y) ((y) * IMG_SIZE + (x))
#define SUB_INDEX(x,y) ((y) * SIZE + (x))
#define CRO_INDEX(x,y) ((y) * CROP_SIZE + (x))
#define CON_INDEX(x,y,tx,ty) ((ty) * CON_SIZE * CROP_SIZE + (tx) * CROP_SIZE + \
        CONST_INDEX((x),(y)))
#define CONST_INDEX(x,y) ((y) * CON_SIZE + (x))

typedef struct Tile{
    long id;
    char image[SIZE*SIZE];
    // First swap, then flip
    int swapXY;
    int flipX;
    int flipY;
    int oriented;
} Tile;

typedef struct Image{
    char *data;
    int swapXY;
    int flipX;
    int flipY;
} Image;

int matches(Tile a, Tile b);
Tile *parse(FILE *in);
void parseLine(char *line);
int getMatchingCount(Tile *all, int one);
char *constructImage(Tile *tiles);
Tile *getCorners(Tile *tiles);
Tile *constructTiles(Tile *tiles, Tile *corners);
void assignOrientationCorner(Tile *corner, Tile *tiles);
Tile getMatchingLeft(Tile *tiles, char *border, int matchId);
Tile getMatchingTop(Tile *tiles, char *border, int matchId);
int matchesBorder(Tile a, char *line);
char **getBorders(Tile tile);
char **applyRotationToBorders(char **borders, Tile tile);
void printTiles(Tile *tiles);
void printTile(Tile tile);
void actualXY(int x, int y, int *actualX, int *actualY, Tile tile);
void actualXY_c(int x, int y, int *actualX, int *actualY, Tile tile);
void actualXY_b(int x, int y, int *actualX, int *actualY, Tile tile);
void actualXY_f(int x, int y, int *actualX, int *actualY, Image image);
void printBorder(char *border);
void printConstructed(char *con);
int markMonsters(Image *image);
char get(Image *image, int x, int y);
void set(Image *image, int x, int y, char val);
void findMonsterOrientation(Image *image);
int countNonMonsters(Image *image);

int main()
{
	FILE *in = fopen("in20", "r");

    Tile *parsed = parse(in);
    Tile *corners = getCorners(parsed);
    long part1 = 1;
    for (int i = 0; i < 4; i++)
        part1 *= corners[i].id;
    printf("Part 1: %ld\n", part1);
    char *image = constructImage(parsed);
    Image theImage = {.data=image, .swapXY=0, .flipX=0, .flipY=0};
    findMonsterOrientation(&theImage);
    long waterRoughness = countNonMonsters(&theImage);
    printf("Water roughness %ld\n", waterRoughness);

    free(parsed);
    free(corners);
    free(image);
	fclose(in);	
	return 0;	
}

int countNonMonsters(Image *image)
{
    int res = 0;
    for (int i = 0; i < CON_SIZE * CON_SIZE; i++)
        if (image->data[i] == 1)
            res++;
    return res;
}

void findMonsterOrientation(Image *image)
{
    for (int a = 0; a < 2; a++)
    for (int b = 0; b < 2; b++)
    for (int c = 0; c < 2; c++)
    {
        image->swapXY = a;
        image->flipX = b;
        image->flipY = c;
        int monsterCount = markMonsters(image);
        if (monsterCount)
            printf("Found %d monster%s!\n", monsterCount, monsterCount > 0 ? "s" : "");
    }
}

int markMonsters(Image *image)
{
    int monsterCount = 0;
    for (int x = 0; x < CON_SIZE - MONSTER_LENGTH; x++)
        for (int y = 0; y < CON_SIZE - MONSTER_HEIGHT; y++)
        {
            if (get(image, x+18, y+0) &&
                get(image, x+0, y+1) &&
                get(image, x+5, y+1) &&
                get(image, x+6, y+1) &&
                get(image, x+11, y+1) &&
                get(image, x+12, y+1) &&
                get(image, x+17, y+1) &&
                get(image, x+18, y+1) &&
                get(image, x+19, y+1) &&
                get(image, x+1, y+2) &&
                get(image, x+4, y+2) &&
                get(image, x+7, y+2) &&
                get(image, x+10, y+2) &&
                get(image, x+13, y+2) &&
                get(image, x+16, y+2))
            {
                monsterCount++;
                set(image, x+18, y+0, 2);
                set(image, x+0, y+1, 2);
                set(image, x+5, y+1, 2);
                set(image, x+6, y+1, 2);
                set(image, x+11, y+1, 2);
                set(image, x+12, y+1, 2);
                set(image, x+17, y+1, 2);
                set(image, x+18, y+1, 2);
                set(image, x+19, y+1, 2);
                set(image, x+1, y+2, 2);
                set(image, x+4, y+2, 2);
                set(image, x+7, y+2, 2);
                set(image, x+10, y+2, 2);
                set(image, x+13, y+2, 2);
                set(image, x+16, y+2, 2);
            }
        }
    return monsterCount;
}

char get(Image *image, int x, int y)
{
    int ax, ay;
    actualXY_f(x, y, &ax, &ay, *image);
    return image->data[CONST_INDEX(ax,ay)];
}

void set(Image *image, int x, int y, char val)
{
    int ax, ay;
    actualXY_f(x, y, &ax, &ay, *image);
    image->data[CONST_INDEX(ax,ay)] = val;
}

void printConstructed(char *con)
{
    for (int y = 0; y < CON_SIZE; y++)
    {
        for (int x = 0; x < CON_SIZE; x++)
            printf("%c", con[CONST_INDEX(x,y)] == 1 
                    ? '#' 
                    : con[CONST_INDEX(x,y)] == 2
                    ? 'O'
                    : '.');
        printf("\n");
    }
}

void printTiles(Tile *tiles)
{
    for (int y = 0; y < IMG_SIZE; y++)
    {
        for (int x = 0; x < IMG_SIZE; x++)
        {
            printTile(tiles[INDEX(x, y)]);
            printf("\n");
        }
        printf("===================\n\n");
    }
}

void printTile(Tile tile)
{
    printf("Tile %ld (%d, %d, %d)\n", tile.id, tile.swapXY, tile.flipX, tile.flipY);
    for (int y = 0; y < SIZE; y++)
    {
        for (int x = 0; x < SIZE; x++)
        {
            int actualX, actualY;
            actualXY(x, y, &actualX, &actualY, tile);
            printf("%c ", tile.image[SUB_INDEX(actualX, actualY)] ? '#' : '.');
        }
        printf("\n");
    }
}

void actualXY_b(int x, int y, int *actualX, int *actualY, Tile tile)
{
    if (tile.swapXY)
    {
        *actualX = y;
        *actualY = x;
    }
    else
    {
        *actualX = x;
        *actualY = y;
    }
}
void actualXY_f(int x, int y, int *actualX, int *actualY, Image image)
{
    if (image.swapXY)
    {
        *actualX = y;
        *actualY = x;
    }
    else
    {
        *actualX = x;
        *actualY = y;
    }
    if (image.flipX && !image.swapXY || image.flipY && image.swapXY)
        *actualX = CON_SIZE - *actualX - 1;
    if (image.flipY && !image.swapXY || image.flipX && image.swapXY)
        *actualY = CON_SIZE - *actualY - 1;
}
void actualXY(int x, int y, int *actualX, int *actualY, Tile tile)
{
    actualXY_b(x, y, actualX, actualY, tile);
    if (tile.flipX && !tile.swapXY || tile.flipY && tile.swapXY)
        *actualX = SIZE - *actualX - 1;
    if (tile.flipY && !tile.swapXY || tile.flipX && tile.swapXY)
        *actualY = SIZE - *actualY - 1;
}

void actualXY_c(int x, int y, int *actualX, int *actualY, Tile tile)
{
    actualXY_b(x, y, actualX, actualY, tile);
    if (tile.flipX && !tile.swapXY || tile.flipY && tile.swapXY)
        *actualX = SIZE - *actualX - 2;
    else
        (*actualX)++;
    if (tile.flipY && !tile.swapXY || tile.flipX && tile.swapXY)
        *actualY = SIZE - *actualY - 2;
    else
        (*actualY)++;
}

char *constructImage(Tile *tiles)
{
    Tile *corners = getCorners(tiles);
    Tile *constructed = constructTiles(tiles, corners);
    printf("\n");
    char *res = malloc(CROP_SIZE * CROP_SIZE * TILE_CNT * sizeof(*res));
    for (int ty = 0; ty < IMG_SIZE; ty++)
    for (int tx = 0; tx < IMG_SIZE; tx++)
    {
        Tile currentTile = constructed[INDEX(tx,ty)];
        for (int y = 0; y < CROP_SIZE; y++)
        {
            for (int x = 0; x < CROP_SIZE; x++)
            {
                int actualX, actualY;
                actualXY_c(x,y,&actualX,&actualY,currentTile);
                res[CON_INDEX(x,y,tx,ty)] = currentTile.image[SUB_INDEX(actualX,actualY)];
            }
        }
    }

    free(corners);
    free(constructed);
    return res;
}

Tile getMatchingTop(Tile *tiles, char *border, int matchId)
{
    for (int i = 0; i < TILE_CNT; i++)
        if (tiles[i].id != matchId)
        {
            int m = matchesBorder(tiles[i], border);
            if (m)
            {
                if (tiles[i].oriented)
                    fprintf(stderr, "That was unexpected\n");
                tiles[i].oriented = 1;
                // TBLR
                m--;
                switch(m)
                {
                    case 0:
                        // Top unflipped
                        break;
                    case 1:
                        // Bottom unflipped
                        tiles[i].flipY = 1;
                        break;
                    case 2:
                        // Left unflipped
                        tiles[i].swapXY = 1;
                        break;
                    case 3:
                        // Right unflipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipY = 1;
                        break;
                    case 4:
                        // Top flipped
                        tiles[i].flipX = 1;
                        break;
                    case 5:
                        // Bottom flipped
                        tiles[i].flipX = 1;
                        tiles[i].flipY = 1;
                        break;
                    case 6:
                        // Left flipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipX = 1;
                        break;
                    case 7:
                        // Right flipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipX = 1;
                        tiles[i].flipY = 1;
                        break;
                    default:
                        fprintf(stderr, "PANIC (%d)\n", m);
                        break;
                }
                return tiles[i];
            }
        }
    fprintf(stderr, "Should've found something\n");
    Tile res = {0};
    return res;
}

Tile getMatchingLeft(Tile *tiles, char *border, int matchId)
{
    for (int i = 0; i < TILE_CNT; i++)
        if (tiles[i].id != matchId)
        {
            int m = matchesBorder(tiles[i], border);
            if (m)
            {
                if (tiles[i].oriented)
                    fprintf(stderr, "That was unexpected\n");
                tiles[i].oriented = 1;
                // TBLR
                m--;
                switch(m)
                {
                    case 0:
                        // Top unflipped
                        tiles[i].swapXY = 1;
                        break;
                    case 1:
                        // Bottom unflipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipX = 1;
                        break;
                    case 2:
                        // Left unflipped
                        break;
                    case 3:
                        // Right unflipped
                        tiles[i].flipX = 1;
                        break;
                    case 4:
                        // Top flipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipY = 1;
                        break;
                    case 5:
                        // Bottom flipped
                        tiles[i].swapXY = 1;
                        tiles[i].flipX = 1;
                        tiles[i].flipY = 1;
                        break;
                    case 6:
                        // Left flipped
                        tiles[i].flipY = 1;
                        break;
                    case 7:
                        // Right flipped
                        tiles[i].flipX = 1;
                        tiles[i].flipY = 1;
                        break;
                    default:
                        fprintf(stderr, "PANIC (%d)\n", m);
                        break;
                }
                return tiles[i];
            }
        }
    fprintf(stderr, "Should've found something\n");
    Tile res = {0};
    return res;
}

Tile *constructTiles(Tile *tiles, Tile *corners)
{
    // Choose a random corner, as we worry about orientation later
    Tile start = *corners;
    Tile *res = malloc(TILE_CNT * sizeof(*res));
    char **borders;
    for (int y = 0; y < IMG_SIZE; y++)
        for (int x = 0; x < IMG_SIZE; x++)
        {
            if (y == 0 && x == 0)
            {
                // First corner
                assignOrientationCorner(&start, tiles);
                res[INDEX(x,y)] = start;
                continue;
            }
            if (x == 0)
            {
                // Fist col
                Tile top = res[INDEX(x,y-1)];
                borders = getBorders(top);
                res[INDEX(x,y)] = getMatchingTop(tiles, borders[1], top.id);
                for (int i = 0; i < 4; i++)
                    free(borders[i]);
                free(borders);
                continue;
            }
            // Something in the middle
            Tile left = res[INDEX(x-1, y)];
            borders = getBorders(left);
            res[INDEX(x,y)] = getMatchingLeft(tiles, borders[3], left.id);
            for (int i = 0; i < 4; i++)
                free(borders[i]);
            free(borders);
        }
    return res;
}

// Orientates the corner as top left
void assignOrientationCorner(Tile *corner, Tile *tiles)
{
    for (int j = 0; j < TILE_CNT; j++)
    {
        if (corner->id != tiles[j].id)
        {
            int m = matches(*corner, tiles[j]);
            if (m > 0)
            {
                m--;
                switch(m%4)
                {
                    case 0:
                        // Top
                        corner->flipY = 1;
                        break;
                    case 1:
                        // Bottom
                        break;
                    case 2:
                        // Left
                        corner->flipX = 1;
                        break;
                    case 3:
                        // Right
                        break;
                }
            }
        }
    }
    corner->oriented = 1;
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

char **getBorders(Tile tile)
{
    char *arr = tile.image;
    char **borders = malloc(4 * sizeof(*borders));
    // Upper border
    borders[0] = malloc(SIZE * sizeof(*borders[0]));
    for (int i = 0; i < SIZE; i++)
        borders[0][i] = arr[SUB_INDEX(i,0)];
    // Bottom border
    borders[1] = malloc(SIZE * sizeof(*borders[1]));
    for (int i = 0; i < SIZE; i++)
        borders[1][i] = arr[SUB_INDEX(i,SIZE-1)];
    // Left border
    borders[2] = malloc(SIZE * sizeof(*borders[2]));
    for (int i = 0; i < SIZE; i++)
        borders[2][i] = arr[SUB_INDEX(0,i)];
    // Right border
    borders[3] = malloc(SIZE * sizeof(*borders[3]));
    for (int i = 0; i < SIZE; i++)
        borders[3][i] = arr[SUB_INDEX(SIZE-1,i)];
    char **res = applyRotationToBorders(borders, tile);
    free(borders);
    return res;
}

char **applyRotationToBorders(char **borders, Tile tile)
{
    char **res = malloc(4 * sizeof(*res));
    if (tile.swapXY)
    {
        res[0] = borders[2];
        res[1] = borders[3];
        res[2] = borders[0];
        res[3] = borders[1];
    }
    else
        memcpy(res, borders, 4*sizeof(*res));
    if (tile.flipX)
    {
        char *buff = malloc(SIZE * sizeof(*buff));
        for (int i = 0; i < SIZE; i++)
            buff[i] = res[0][SIZE-i-1];
        memcpy(res[0], buff, SIZE * sizeof(*res[0]));
        for (int i = 0; i < SIZE; i++)
            buff[i] = res[1][SIZE-i-1];
        memcpy(res[1], buff, SIZE * sizeof(*res[1]));
        free(buff);
        buff = res[2];
        res[2] = res[3];
        res[3] = buff;
    }
    if (tile.flipY)
    {
        char *buff = malloc(SIZE * sizeof(*buff));
        for (int i = 0; i < SIZE; i++)
            buff[i] = res[2][SIZE-i-1];
        memcpy(res[2], buff, SIZE * sizeof(*res[2]));
        for (int i = 0; i < SIZE; i++)
            buff[i] = res[3][SIZE-i-1];
        memcpy(res[3], buff, SIZE * sizeof(*res[3]));
        free(buff);
        buff = res[0];
        res[0] = res[1];
        res[1] = buff;
    }
    return res;
}

// Returns which border+1 (UBLR) + 4 iff flipped matched, or 0 otherwise
int matches(Tile a, Tile b)
{
    char **bordersB = getBorders(b);
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
    int i;
    for (i = 0; i < SIZE; i++)
        if (border[i] != border[SIZE-i-1])
            break;
    if (i == SIZE)
    {
        fprintf(stderr, "Found symmetric border\n");
        printBorder(border);
    }
    char **bordersA = getBorders(a);
    int res = 0;
    for (i = 0; i < 4; i++)
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

void printBorder(char *border)
{
    for (int i = 0; i < SIZE; i++)
        printf("%c", border[i] ? '#' : '.');
    printf("\n");
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

