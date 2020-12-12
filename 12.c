#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_COUNT 755
#define LINE_LENGTH 5

size_t iterations = 0;
int manhattan(int x, int y);
int *simulate(char **instructions);
int *simulate2(char **instructions);
void rotateLeft (int *wx, int *wy);

char **read(FILE *in);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in12", "r");

	char **lines = read(in);
	int *coords = simulate(lines);
	printf("Distance: %d\n", manhattan(coords[0], coords[1]));
	int *coords2 = simulate2(lines);
	printf("Distance 2: %d\n", manhattan(coords2[0], coords2[1]));

	free(coords2);
	free(coords);
	for (int i = 0; i < LINE_COUNT; i++)
		free(lines[i]);
	free(lines);
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
		*strchr(res[i], '\n') = '\0';
	}

	return res;
}

int *simulate(char **instructions)
{
	int *coords = calloc(2, sizeof(*coords));
	int *x = coords, *y = coords+1;
	// 0 -> east, 1 -> south, 2 -> west, 3 -> north
	int direction = 0;
	for (int i = 0; i < LINE_COUNT; i++)
	{
		int val = atoi(instructions[i]+1);
		switch(instructions[i][0])
		{
			case 'N':
				*y+=val;
				break;
			case 'S':
				*y-=val;
				break;
			case 'E':
				*x+=val;
				break;;
			case 'W':
				*x-=val;
				break;
			case 'L':
				direction = (direction - val / 90 + 4) % 4;
				break;
			case 'R':
				direction = (direction + val / 90) % 4;
				break;
			case 'F':
				switch(direction)
				{
					case 0:
						*x+=val;
						break;
					case 1:
						*y-=val;
						break;
					case 2:
						*x-=val;
						break;
					case 3:
						*y+=val;
						break;
				}
				break;
		}
	}

	return coords;
}

int *simulate2(char **instructions)
{
	int *coords = calloc(2, sizeof(*coords));
	int *x = coords, *y = coords+1;
	int wx = 10, wy = 1;

	for (int i = 0; i < LINE_COUNT; i++)
	{
		int val = atoi(instructions[i]+1);
		switch(instructions[i][0])
		{
			case 'N':
				wy+=val;
				break;
			case 'S':
				wy-=val;
				break;
			case 'E':
				wx+=val;
				break;;
			case 'W':
				wx-=val;
				break;
			case 'L':
				for (int j = 0; j < val / 90; j++)
					rotateLeft(&wx, &wy);
				break;
			case 'R':
				for (int j = 0; j < 4 - val / 90; j++)
					rotateLeft(&wx, &wy);
				break;
			case 'F':
				*x += val * wx;
				*y += val * wy;
				break;
		}
	}

	return coords;
}

void rotateLeft (int *wx, int *wy)
{
	int b = -*wy;
	*wy = *wx;
	*wx = b;
}

int manhattan(int x, int y)
{
	return (x < 0 ? -x : x) + (y < 0 ? -y : y);
}
