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
int isNumberN(char *content, int n);
int isDigit(char c);
int isSuperValid(char *field);

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
	loop:
	while ((fields = getPassportFields(in)) != NULL)
	{
		if (*fields == NULL)
			// Not enough fields
			continue;

		for (int i = 0; i < FIELDS_COUNT; i++)
			if (!isSuperValid(fields[i]))
			{
				// Passport is invalid - we don't need to check the rest
				free(fields);
				goto loop;
			}

		count++;
		free(fields);
	}
	return count;
}

int isSuperValid(char *field)
{
	if (field == NULL)
		// cid must be missing - we already validated that all other fields must be present
		return 1;
	int validated = 0;
	char *sub = calloc(3, sizeof(*sub));;
	memcpy(sub, field, 3);

	if (strcmp(sub, "byr") == 0)
		return validateBirthyear(field+4);
	if (strcmp(sub, "iyr") == 0)
		return validateIssueYear(field+4);
	if (strcmp(sub, "eyr") == 0)
		return validateExpirationYear(field+4);
	if (strcmp(sub, "ecl") == 0)
		return validateEyeColor(field+4);
	if (strcmp(sub, "hgt") == 0)
		return validateHeight(field+4);
	if (strcmp(sub, "hcl") == 0)
		return validateHairColor(field+4);
	if (strcmp(sub, "pid") == 0)
		return validatePassportId(field+4);
	if (strcmp(sub, "cid") == 0)
		return validateCountry(field+4);
	fprintf(stderr, "Unexpected: %s\n", sub);
	return 0;
	free(field);
}

char **getPassportFields(FILE *in)
{
	char *passport = getNextPassport(in);
	// Needed for free
	char *originalPassportPointer = passport;

	if (passport == NULL)
		// Last passport has already passed
		return NULL;

	char **fields = calloc(FIELDS_COUNT, sizeof(*fields));
	// We don't need _all_ fields - one may be missing
	if (getValidParamCount(passport) != FIELDS_COUNT - 1)
	{
		free(passport);
		// This will return a pointer to a NULL pointer
		return fields;
	}

	char *currentLine = calloc(MAX_LINE_LENGTH, sizeof(*currentLine));
	int j = 0, i = 0;
	for (; passport[i] != '\0'; i++)
	{
		if (!isDelimiter(passport[i]))
		{
			currentLine[i] = passport[i];
			continue;
		}

		if (i > 0)
		{
			fields[j] = currentLine;
			currentLine = calloc(MAX_LINE_LENGTH, sizeof(*currentLine));
			j++;
		}
		passport += i + 1;
		i = -1;
	}

	if (i > 0)
	{
		printf("Got one last line\n");
		fields[j] = currentLine;
	}
	else
		free(currentLine);

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
			// We're done
			break;

		sprintf(passport + read, "%s", currentLine);
		read += strlen(currentLine);
		lineLength = 0;
		currentLine = NULL;
	}

	if (read == 0)
	{
		// Got nothing
		free(passport);
		return NULL;
	}

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
		return !(sscanf(content, "%dcm", &height) <= 0 || strlen(content) != 5 || !isNumberN(content, 3)) &&
			height >= 150 && height <= 193;
	if (strstr(content, "in") != NULL)
		return !(sscanf(content, "%din", &height) <= 0 || strlen(content) != 4 || !isNumberN(content, 2)) &&
			height >= 59 && height <= 76;

	return 0;
}

int validateHairColor(char *content)
{
	if (content[0] != '#')
		return 0;
	if (strlen(content) != 7)
		return 0;
	for (int i = 1; i < 7; i++)
		if (!isDigit(content[i]) && (content[i] < 'a' || content[i] > 'f'))
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
	return isNumberN(content, strlen(content));
}

int isNumberN(char *content, int n)
{
	for (int i = 0; i < n; i++)
		if (!isDigit(content[i]))
			return 0;
	return 1;
}

int isDigit(char c)
{
	return c >= '0' && c <= '9';
}
