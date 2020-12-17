#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Only in x,y - in z, initially there's only one dimension
#define INITIAL_DIMENSIONS 8
#define CYCLE_COUNT 6
#define FINAL_DIMENSIONS (INITIAL_DIMENSIONS + 2 * CYCLE_COUNT)

size_t iterations = 0;

int *getInitialActiveStates(FILE *in);
int getIndex(int x, int y, int z, int cycle);
int getValue(int *states, int x, int y, int z, int cycle);
void iterate(int *states, int cycle);
int getActiveNeighbors(int *states, int x, int y, int z, int cycle);
int countActive(int *states);
void print(int *states, int cycle);
int *getInitialActiveStates4(FILE *in);
int getIndex4(int x, int y, int z, int w, int cycle);
int getValue4(int *states, int x, int y, int z, int w, int cycle);
void iterate4(int *states, int cycle);
int getActiveNeighbors4(int *states, int x, int y, int z, int w, int cycle);
int countActive4(int *states);
void print4(int *states, int cycle);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in17", "r");

	int *initial = getInitialActiveStates(in);
	//print(initial, 0);
	for (int i = 1; i <= CYCLE_COUNT; i++)
	{
		iterate(initial, i);
		//printf("------------------------\ncycle=%d\n------------------------\n", i);
		//print(initial, i);
	}
	printf("Active: %d\n", countActive(initial));
	rewind(in);
	int *initial4 = getInitialActiveStates4(in);
	//print4(initial4, 0);
	for (int i = 1; i <= CYCLE_COUNT; i++)
	{
		iterate4(initial4, i);
		//printf("------------------------\ncycle=%d\n------------------------\n", i);
		//print(initial4, i);
	}
	printf("Active: %d\n", countActive4(initial4));
	rewind(in);

	free(initial);
	free(initial4);
	fclose(in);	
	return 0;	
}

int *getInitialActiveStates(FILE *in)
{	
	char *line = NULL;
	size_t n = 0;
	// Avoid reallocs by creating a res big enough
	int *res = calloc(FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1), sizeof(*res));

	int j = 0;
	while (getline(&line, &n, in) >= 0 && line[0] != '\0' && line[0] != '\n')
	{
		for (int i = 0; i < INITIAL_DIMENSIONS; i++)
			res[getIndex(i, j, 0, 0)] = line[i] == '#';
		j++;
	}

	free(line);

	return res;
}

int getIndex(int x, int y, int z, int cycle)
{
	int res = ((CYCLE_COUNT - cycle) + z) * FINAL_DIMENSIONS * FINAL_DIMENSIONS +
		   ((CYCLE_COUNT - cycle) + y) * FINAL_DIMENSIONS +
		   (CYCLE_COUNT - cycle) + x;
	if (res < 0 || res >= FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1))
		fprintf(stderr, "Invalid index: %d (%d, %d, %d, %d) - max: %d\n", res, x, y, z, cycle, FINAL_DIMENSIONS * FINAL_DIMENSIONS * (CYCLE_COUNT + 1));
	return res;
}

int getValue(int *states, int x, int y, int z, int cycle)
{
	return
		!(x < 0 || x >= INITIAL_DIMENSIONS + 2 * cycle ||
		  y < 0 || y >= INITIAL_DIMENSIONS + 2 * cycle ||
		  z < 0 || z >= 1 + 2 * cycle) && 
		states[getIndex(x, y, z, cycle)];
}

void iterate(int *states, int cycle)
{
	int *newStates = calloc(FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1), sizeof(*newStates));
	if (newStates == NULL)
	{
		fprintf(stderr, "Alloc failed\n");
		exit(1);
	}
	for (int x = 0; x < INITIAL_DIMENSIONS + 2 * cycle; x++)
		for (int y = 0; y < INITIAL_DIMENSIONS + 2 * cycle; y++)
			for (int z = 0; z < 1 + 2 * cycle; z++)
			{
				int activeNeighbors = getActiveNeighbors(states, x, y, z, cycle);
				newStates[getIndex(x, y, z, cycle)] = 
					activeNeighbors == 3 ||
					activeNeighbors == 2 && states[getIndex(x, y, z, cycle)];
			}
	memcpy(states, newStates, FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * sizeof(*states));
	free(newStates);
}

int getActiveNeighbors(int *states, int x, int y, int z, int cycle)
{
	int res = 0;
	for (int xi = x-1; xi <= x+1; xi++)
		for (int yi = y-1; yi <= y+1; yi++)
			for (int zi = z-1; zi <= z+1; zi++)
				res += (xi != x || yi != y || zi != z) && 
						getValue(states, xi, yi, zi, cycle);
	return res;
}

int countActive(int *states)
{
	int res = 0;
	for (int i = 0; i < FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1); i++)
		res += states[i];
	return res;
}

void print(int *states, int cycle)
{
	for (int z = 0; z < (2 * CYCLE_COUNT + 1) - 2 * (CYCLE_COUNT - cycle); z++)
	{
		printf("z=%d\n ", z);
		for (int x = 0; x < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); x++)
			printf("%2d", x);
		printf("\n");
		for (int y = 0; y < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); y++)
		{
			printf("%2d", y);
			for (int x = 0; x < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); x++)
				printf("%c ", states[getIndex(x, y, z, cycle)] ? '#' : '.');
			printf("\n");
		}
		printf("\n");
	}	
}

// 4

int *getInitialActiveStates4(FILE *in)
{	
	char *line = NULL;
	size_t n = 0;
	// Avoid reallocs by creating a res big enough
	int *res = calloc(FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1), 
		sizeof(*res));

	int j = 0;
	while (getline(&line, &n, in) >= 0 && line[0] != '\0' && line[0] != '\n')
	{
		for (int i = 0; i < INITIAL_DIMENSIONS; i++)
			res[getIndex4(i, j, 0, 0, 0)] = line[i] == '#';
		j++;
	}

	free(line);

	return res;
}

int getIndex4(int x, int y, int z, int w, int cycle)
{
	int res =
		   ((CYCLE_COUNT - cycle) + w) * FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) +
		   ((CYCLE_COUNT - cycle) + z) * FINAL_DIMENSIONS * FINAL_DIMENSIONS +
		   ((CYCLE_COUNT - cycle) + y) * FINAL_DIMENSIONS +
		   (CYCLE_COUNT - cycle) + x;
	if (res < 0 || res >= FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1))
		fprintf(stderr, "Invalid index: %d (%d, %d, %d, %d, %d) - max: %d\n", res, x, y, z, w, cycle,
			FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1));
	return res;
}

int getValue4(int *states, int x, int y, int z, int w, int cycle)
{
	return
		!(x < 0 || x >= INITIAL_DIMENSIONS + 2 * cycle ||
		  y < 0 || y >= INITIAL_DIMENSIONS + 2 * cycle ||
		  z < 0 || z >= 1 + 2 * cycle ||
		  w < 0 || w >= 1 + 2 * cycle) && 
		states[getIndex4(x, y, z, w, cycle)];
}

void iterate4(int *states, int cycle)
{
	int *newStates = calloc(FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1), 
		sizeof(*newStates));
	if (newStates == NULL)
	{
		fprintf(stderr, "Alloc failed\n");
		exit(1);
	}
	for (int x = 0; x < INITIAL_DIMENSIONS + 2 * cycle; x++)
	for (int y = 0; y < INITIAL_DIMENSIONS + 2 * cycle; y++)
	for (int z = 0; z < 1 + 2 * cycle; z++)
	for (int w = 0; w < 1 + 2 * cycle; w++)
	{
		int activeNeighbors = getActiveNeighbors4(states, x, y, z, w, cycle);
		newStates[getIndex4(x, y, z, w, cycle)] = 
			activeNeighbors == 3 ||
			activeNeighbors == 2 && states[getIndex4(x, y, z, w, cycle)];
	}
	memcpy(states, newStates, FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1)
		 * sizeof(*states));
	free(newStates);
}

int getActiveNeighbors4(int *states, int x, int y, int z, int w, int cycle)
{
	int res = 0;
	for (int xi = x-1; xi <= x+1; xi++)
	for (int yi = y-1; yi <= y+1; yi++)
	for (int zi = z-1; zi <= z+1; zi++)
	for (int wi = w-1; wi <= w+1; wi++)
		res += (xi != x || yi != y || zi != z || wi != w) && 
			getValue4(states, xi, yi, zi, wi, cycle);
	return res;
}

int countActive4(int *states)
{
	int res = 0;
	for (int i = 0; i < FINAL_DIMENSIONS * FINAL_DIMENSIONS * (2 * CYCLE_COUNT + 1) * (2 * CYCLE_COUNT + 1); i++)
		res += states[i];
	return res;
}

void print4(int *states, int cycle)
{
	for (int w = 0; w < (2 * CYCLE_COUNT + 1) - 2 * (CYCLE_COUNT - cycle); w++)
	{
		printf("================\nw=%d\n================\n", w);
		for (int z = 0; z < (2 * CYCLE_COUNT + 1) - 2 * (CYCLE_COUNT - cycle); z++)
		{
			printf("z=%d\n ", z);
			for (int x = 0; x < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); x++)
				printf("%2d", x);
			printf("\n");
			for (int y = 0; y < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); y++)
			{
				printf("%2d", y);
				for (int x = 0; x < FINAL_DIMENSIONS - 2 * (CYCLE_COUNT - cycle); x++)
					printf("%c ", states[getIndex(x, y, z, cycle)] ? '#' : '.');
				printf("\n");
			}
			printf("\n");
		}
	}
}
