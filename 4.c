#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINES 1030
#define MAX_PASSPORT_LENGTH 1000
#define FIELDS_COUNT 8
#define MAX_LINE_LENGTH 100

int getValidCount(FILE *in);
char *getNextPassport(FILE *in);
// Returns NULL if the passport isn't valid
char **getPassportFields(FILE *in);
int isDelimiter(char c);
int isEndOfLine(char c);
int getSuperValidCount(FILE *in);
int getValidParamCount(char *passport);
int validateBirthyear(char *content);
int validateIssueYear(char *content);
int validateExpirationYear(char *content);
int validateHeight(char *content);
int validateHairColor(char *content);
int validateEyeColor(char *content);
int validatePassportId(char *content);
int validateCountry(char *content);
int isNumber(char *content);
int isDigit(char c);

int main (int argc, char *argv[])
{
	FILE *in = fopen("in4", "r");

	int validCount = getValidCount(in);
	rewind(in);
	int superValidCount = getSuperValidCount(in);

	printf("Valid count is: %d\nSuper valid count is: %d\n", validCount, superValidCount);

	fclose(in);
	return 0;
}

int getSuperValidCount(FILE *in)
{
	char **fields;
	int count = 0;
	while ((fields = getPassportFields(in)) != NULL)
	{
		if (*fields == NULL)
		{
			printf("Not enough params\n");
			continue;
		}
		int valid = 1;
		for (int i = 0; i < FIELDS_COUNT; i++)
		{
			if (fields[i] == NULL)
				// cid must be missing
				continue;
			int validated = 0;
			switch(fields[i][0])
			{
				case 'b':
					if (!validateBirthyear(fields[i]+4))
					{
						printf("%s is invalid\n", fields[i]);
						valid = 0;
						continue;
					}
					else
						printf("%s is valid\n", fields[i]);
					break;
				case 'i':
					if (!validateIssueYear(fields[i]+4))
					{
						printf("%s is invalid\n", fields[i]);
						valid = 0;
						continue;
					}
					else
						printf("%s is valid\n", fields[i]);
					break;
				case 'e':
					switch(fields[i][1])
					{
						case 'y':
							if (!validateExpirationYear(fields[i]+4))
							{
								printf("%s is invalid\n", fields[i]);
								valid = 0;
								continue;
							}
							else
								printf("%s is valid\n", fields[i]);
							break;
						case 'c':
							if (!validateEyeColor(fields[i]+4))
							{
								printf("%s is invalid\n", fields[i]);
								valid = 0;
								continue;
							}
							else
								printf("%s is valid\n", fields[i]);
							break;
						default:
							fprintf(stderr, "Unexpected: %c\n", fields[i][1]);
							return -1;
					}
					break;
				case 'h':
					switch(fields[i][1])
					{
						case 'g':
							if (!validateHeight(fields[i]+4))
							{
								printf("%s is invalid\n", fields[i]);
								valid = 0;
								continue;
							}
							else
								printf("%s is valid\n", fields[i]);
							break;
						case 'c':
							if (!validateHairColor(fields[i]+4))
							{
								printf("%s is invalid\n", fields[i]);
								valid = 0;
								continue;
							}
							else
								printf("%s is valid\n", fields[i]);
							break;
						default:
							fprintf(stderr, "Unexpected: %c\n", fields[i][1]);
							valid = 0;
							return -1;
					}					
					break;
				case 'p':
					if (!validatePassportId(fields[i]+4))
					{
						printf("%s is invalid\n", fields[i]);
						valid = 0;
						continue;
					}
					else
						printf("%s is valid\n", fields[i]);
					break;
				case 'c':
					if (!validateCountry(fields[i]+4))
					{
						printf("%s is invalid\n", fields[i]);
						valid = 0;
						continue;
					}
					else
						printf("%s is valid\n", fields[i]);
					break;
				default:
					fprintf(stderr, "Unexpected: %c\n", fields[i][0]);
					return -1;
			}
			// TODO: Figure out why free doesn't work
			// free(fields[i]);
		}

		if (valid)
			printf("Was valid\n\n");
		else
			printf("Was invalid\n\n");
		count+=valid;
		// free(fields);
	}
	return count;
}

char **getPassportFields(FILE *in)
{
	char *passport = getNextPassport(in);
	// printf("Got passport %s\n", passport);
	char *originalPassportPointer = passport;
	if (passport == NULL)
		return NULL;
	char **fields = (char **) calloc(FIELDS_COUNT, sizeof(*fields));
	if (getValidParamCount(passport) != 7)
	{
		free(passport);
		return fields;
	}
	char *currentLine = calloc(MAX_LINE_LENGTH, sizeof(*currentLine));
	int j = 0, i = 0;
	for (; passport[i] != '\0'; i++)
	{
		if (isDelimiter(passport[i]))
		{
			if (i > 0)
			{
				// printf("currentLine: %s\n", currentLine);
				fields[j] = currentLine;
				currentLine = calloc(MAX_LINE_LENGTH, sizeof(*currentLine));
				j++;
			}
			passport += i + 1;
			i = -1;
			continue;
		}

		currentLine[i] = passport[i];
	}

	if (i > 0)
	{
		printf("Got one last line\n");
		fields[j] = currentLine;
	}
	else
		free(currentLine);

	// printf("Freeing passport\n");
	free(originalPassportPointer);

	return fields;
}

char *getNextPassport(FILE *in)
{
	char *passport = malloc(MAX_PASSPORT_LENGTH * sizeof(*passport));
	char *currentLine = NULL;
	size_t read = 0;
	size_t lineLength = 0;
	while (getline(&currentLine, &lineLength, in) > 0)
	{
		if (isEndOfLine(*currentLine))
		{
			if (*currentLine == '\0')
			{
				printf("End of line\n");
				break;
			}
			if (read == 0)
			{
				printf("Didn't read anything yet\n");
				continue;
			}
			// printf("Read something, we're done\n");
			break;
		}

		// printf("Got line of length %ld: %s\n", strlen(currentLine), currentLine);

		sprintf(passport + read, "%s", currentLine);
		read += strlen(currentLine);
		lineLength = 0;
		currentLine = NULL;
	}

	if (read == 0)
	{
		printf("Got nothing\n");
		free(passport);
		return NULL;
	}

	// printf("Got something\n");

	return passport;
}

int isDelimiter(char c)
{
	return isEndOfLine(c) || c == ' ';
}

int isEndOfLine(char c)
{
	return c == '\0' || c == '\n' || c == '\r';
}

int getValidCount(FILE *in)
{
	char *currentLine;
	size_t lineLength = 0;
	int currentParamCount = 0;
	int validCount = 0;

	while (getline(&currentLine, &lineLength, in) > 0)
	{
		if (*currentLine == '\0' || *currentLine == '\n' || *currentLine == '\r')
		{
			// Blank line -> new passport
			if (currentParamCount == 7)
				validCount++;
			currentParamCount = 0;
			continue;
		}

		currentParamCount += getValidParamCount(currentLine);
	}

	if (currentParamCount == 7)
		validCount++;

	return validCount;
}

int getValidParamCount(char *passport)
{
	char *required[] = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"/*, "cid"*/};
	int currentParamCount = 0;

	for (int i = 0; i < sizeof(required) / sizeof(*required); i++)
		if (strstr(passport, required[i]) != NULL)
			currentParamCount++;

	return currentParamCount;
}

int validateBirthyear(char *content) 
{
	int year;
	if (!isNumber(content))
		return 0;
	if (sscanf(content, "%d", &year) <= 0)
		return 0;
	return year >= 1920 && year <= 2002;
}
int validateIssueYear(char *content)
{
	int year;
	if (!isNumber(content))
		return 0;
	if (sscanf(content, "%d", &year) <= 0)
		return 0;
	return year >= 2010 && year <= 2020;	
}
int validateExpirationYear(char *content)
{
	int year;
	if (!isNumber(content))
		return 0;
	if (sscanf(content, "%d", &year) <= 0)
		return 0;
	return year >= 2020 && year <= 2030;	
}
int validateHeight(char *content)
{
	int height;
	if (strstr(content, "cm") != NULL)
	{
		if (sscanf(content, "%dcm", &height) <= 0)
			return 0;
		if (strlen(content) != 5)
			return 0;
		if (!isDigit(content[0]) || !isDigit(content[1]) || !isDigit(content[2]))
			return 0;
		return height >= 150 && height <= 193;			
	}
	if (strstr(content, "in") != NULL)
	{
		if (sscanf(content, "%din", &height) <= 0)
			return 0;
		if (strlen(content) != 4)
			return 0;
		if (!isDigit(content[0]) || !isDigit(content[1]))
			return 0;
		return height >= 59 && height <= 76;		
	}

	return 0;
}

int validateHairColor(char *content)
{
	if (content[0] != '#')
		return 0;
	if (strlen(content) != 7)
		return 0;
	for (int i = 1; i < 7; i++)
		if ((content[i] < '0' || content[i] > '9') && (content[i] < 'a' || content[i] > 'f'))
			return 0;

	return 1;
}
int validateEyeColor(char *content)
{
	char *valid[7] = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
	if (strlen(content) != 3)
		return 0;
	for (int i = 0; i < 7; i++)
		if (strstr(content, valid[i]) != NULL)
			return 1;

	return 0;
}
int validatePassportId(char *content)
{
	if (strlen(content) != 9)
		return 0;
	return isNumber(content);
}
int validateCountry(char *content)
{
	return 1;
}

int isNumber(char *content)
{
	for (int i = 0; i < strlen(content); i++)
		if (!isDigit(content[i]))
			return 0;
	return 1;
}

int isDigit(char c)
{
	return c >= '0' && c <= '9';
}
