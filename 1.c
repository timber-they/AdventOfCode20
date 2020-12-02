#include <stdio.h>

int main (int argc, char *argv[])
{
	FILE *in = fopen("in1", "r");
	long count = 0;
	int total = 201;

	long all[total];
	for (int i = 0; i < total; i++)
	{
		fscanf(in, "%ld\n", all+i);
	}

	for (int i = 0; i < total; i++)
	{
		for (int j = i + 1; j < total; j++)
		{
			for (int k = j + 1; k < total; k++)
			{
				count++;
				if (all[i] + all[j] + all[k] == 2020)
				{
					printf("Result: %ld*%ld*%ld=%ld (count: %ld)\n", all[i], all[j], all[k], all[i] * all[j] * all[k], count);
					// fclose(in);
					// return 0;
				}
			}
		}
	}

	printf("Count: %ld\n", count);

	fclose(in);
	return 0;
}