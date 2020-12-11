#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 99
#define LINE_LENGTH 95

size_t iterations = 0;
char **read(FILE *in);
int getAdjacentOccupied(int x, int y, char **map);
int getVisibleOccupied(int x, int y, char **map);
int simulate(char **map);
int simulate2(char **map);
int countOccupied(char **map);
void clear(char **map);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in11", "r");

	char **map = read(in);
	while(simulate(map))
		; // Ignored
	printf("Occupied: %d\n", countOccupied(map));
	clear(map);
	while(simulate2(map))
		; // Ignored
	printf("Occupied 2: %d\n", countOccupied(map));
	printf("Iterations: %ld\n", iterations);

	for (int i = 0; i < LINE_COUNT; i++)
		free(map[i]);
	free(map);
	fclose(in);	
	return 0;	
}

char **read(FILE *in)
{
	char **res = calloc(LINE_COUNT, sizeof(*res));
	for (int i = 0; i < LINE_COUNT; i++)
	{
		iterations++;
		size_t n = LINE_LENGTH+1;
		if (getline(res+i, &n, in) < 0 || res[i][0] == '\0' || res[i][0] == '\n')
			return res;
		res[i][LINE_LENGTH] = '\0';
	}

	return res;
}

int getAdjacentOccupied(int x, int y, char **map)
{
	int res = 0;
	if (x > 0)
	{
		res += map[y][x-1] == '#' || map[y][x-1] == '-';
		if (y > 0)
			res += map[y-1][x-1] == '#' || map[y-1][x-1] == '-';
		if (y < LINE_COUNT - 1)
			res += map[y+1][x-1] == '#' || map[y+1][x-1] == '-';
	}
	if (x < LINE_LENGTH - 1)
	{
		res += map[y][x+1] == '#' || map[y][x+1] == '-';
		if (y > 0)
			res += map[y-1][x+1] == '#' || map[y-1][x+1] == '-';
		if (y < LINE_COUNT - 1)
			res += map[y+1][x+1] == '#' || map[y+1][x+1] == '-';
	}
	if (y > 0)
		res += map[y-1][x] == '#' || map[y-1][x] == '-';
	if (y < LINE_COUNT - 1)
		res += map[y+1][x] == '#' || map[y+1][x] == '-';
	return res;
}

int getVisibleOccupied(int x, int y, char **map)
{
	int res = 0;
	for (int xi = x+1; xi < LINE_LENGTH && map[y][xi] != 'L'; xi++)
	{
		iterations++;
		if (map[y][xi] == '#' || map[y][xi] == '-')
		{
			res++;
			break;
		}
	}
	for (int yi = y+1; yi < LINE_COUNT && map[yi][x] != 'L'; yi++)
	{
		iterations++;
		if (map[yi][x] == '#' || map[yi][x] == '-')
		{
			res++;
			break;
		}
	}
	for (int xi = x-1; xi >= 0 && map[y][xi] != 'L'; xi--)
	{
		iterations++;
		if (map[y][xi] == '#' || map[y][xi] == '-')
		{
			res++;
			break;
		}
	}
	for (int yi = y-1; yi >= 0 && map[yi][x] != 'L'; yi--)
	{
		iterations++;
		if (map[yi][x] == '#' || map[yi][x] == '-')
		{
			res++;
			break;
		}
	}
	for (int xi = x+1, yi = y+1; xi < LINE_LENGTH && yi < LINE_COUNT && map[yi][xi] != 'L'; xi++)
	{
		iterations++;
		if (map[yi][xi] == '#' || map[yi][xi] == '-')
		{
			res++;
			break;
		}
		yi++;
	}
	for (int xi = x+1, yi = y-1; xi < LINE_LENGTH && yi >= 0 && map[yi][xi] != 'L'; xi++)
	{
		iterations++;
		if (map[yi][xi] == '#' || map[yi][xi] == '-')
		{
			res++;
			break;
		}
		yi--;
	}
	for (int xi = x-1, yi = y+1; xi >= 0 && yi < LINE_COUNT && map[yi][xi] != 'L'; xi--)
	{
		iterations++;
		if (map[yi][xi] == '#' || map[yi][xi] == '-')
		{
			res++;
			break;
		}
		yi++;
	}
	for (int xi = x-1, yi = y-1; xi >= 0 && yi >= 0 && map[yi][xi] != 'L'; xi--)
	{
		iterations++;
		if (map[yi][xi] == '#' || map[yi][xi] == '-')
		{
			res++;
			break;
		}
		yi--;
	}
	return res;
}

int simulate(char **map)
{
	int changes = 0;
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)
		{
			iterations++;
			if (map[y][x] == '.')
				continue;
			int adjacent = getAdjacentOccupied(x, y, map);
			//printf("Adjacent: %d\n", adjacent);
			if (adjacent == 0 && map[y][x] == 'L')
			{
				changes++;
				map[y][x] = '+';
			}
			else if(adjacent >= 4 && map[y][x] == '#')
			{
				changes++;
				map[y][x] = '-';
			}
		}
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)
		{
			iterations++;
			switch(map[y][x])
			{
				case '+':
					map[y][x] = '#';
					break;
				case '-':
					map[y][x] = 'L';
					break;
			}
		}
	return changes;
}

int simulate2(char **map)
{
	int changes = 0;
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)
		{
			iterations++;
			if (map[y][x] == '.')
				continue;
			int adjacent = getVisibleOccupied(x, y, map);
			//printf("Adjacent: %d\n", adjacent);
			if (adjacent == 0 && map[y][x] == 'L')
			{
				changes++;
				map[y][x] = '+';
			}
			else if(adjacent >= 5 && map[y][x] == '#')
			{
				changes++;
				map[y][x] = '-';
			}
		}
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)
		{
			iterations++;
			switch(map[y][x])
			{
				case '+':
					map[y][x] = '#';
					break;
				case '-':
					map[y][x] = 'L';
					break;
			}
		}
	return changes;
}

int countOccupied(char **map)
{
	int occupied = 0;
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)	
		{
			iterations++;
			occupied += map[y][x] == '#';
		}
	return occupied;
}

void clear(char **map)
{
	for (int x = 0; x < LINE_LENGTH; x++)
		for (int y = 0; y < LINE_COUNT; y++)
			map[y][x] = map[y][x] == '#' ? 'L' : map[y][x];
}
