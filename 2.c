#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isValid(int min, int max, char c, char *pwd);
int isValid2(int a, int b, char c, char *pwd);

int main (int argc, char *argv[])
{
	FILE *in = fopen("in2", "r");
	const int total = 1000;

	int a[total];
	int b[total];
	char c[total];
	char *pwd[total];

	int valid = 0;

	for (int i = 0; i < total; i++)
	{
		pwd[i] = malloc(50 * sizeof(*pwd[i]));
		fscanf(in, "%d-%d %c: %s\n", a+i, b+i, c+i, pwd[i]);

		if (isValid2(a[i], b[i], c[i], pwd[i]))
		{
			valid++;
			printf("Valid: %d-%d %c: %s\n", a[i], b[i], c[i], pwd[i]);
		}

		free(pwd[i]);
	}

	printf("Count: %d\n", valid);

	fclose(in);
	return 0;
}

int isValid(int min, int max, char c, char *pwd)
{
	int curr = 0;
	for (; *pwd != '\0'; pwd++)
	{
		if (*pwd == c)
			curr++;
		if (curr > max)
			return 0;
	}
	return curr >= min;
}

int isValid2(int a, int b, char c, char *pwd)
{
	return strlen(pwd) >= b && (pwd[a-1] == c) ^ (pwd[b-1] == c);
}