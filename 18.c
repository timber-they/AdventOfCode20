#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;
long part1(FILE *in);
// Start inclusive, end exclusive
long calculate(char *line, int start, int end);
int getEndIndexBracket(char *line, int bracketPos);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in18", "r");

    long res1 = part1(in);
    printf("Part 1: %ld\n", res1);

	fclose(in);	
	return 0;	
}

long part1(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    long res = 0;
    while (getline(&line, &n, in) > 0 && *line != '\n' && *line != '\0')
        res += calculate(line, 0, strlen(line));
    return res;
}

long calculate(char *line, int start, int end)
{
    //printf("Calculate %d to %d\n", start, end);
    long res = 0;
    // 0 -> Nothing, 1 -> Add, 2 -> Multiply
    int lastOperator = 0;
    for (int i = start; i < end; i++)
    {
        long atom = -1;
        int endIndex;
        //printf("Switching %c\n", line[i]);
        switch(line[i])
        {
            case ' ':
                // Separates atoms
                continue;
            case '\n':
                continue;
            case '+':
                lastOperator = 1;
                continue;
            case '*':
                lastOperator = 2;
                continue;
            case '(':
                endIndex = getEndIndexBracket(line, i);
                atom = calculate(line, i+1, endIndex);
                i = endIndex+1;
                break;
            case ')':
                fprintf(stderr, "Closing brackets should be excluded by end\n");
                return -1;
            default:
                //printf("Got value: %c\n", line[i]);
                atom = line[i] - '0';
                break;
        }
        //printf("Using operator %d with %ld on %d", lastOperator, atom, res);
        switch(lastOperator)
        {
            case 0:
                res = atom;
                break;
            case 1:
                res += atom;
                break;
            case 2:
                res *= atom;
                break;
            default:
                fprintf(stderr, "Invalid operator: %d\n", lastOperator);
                break;
        }
        //printf(", resulting in %d\n", res);
    }
    return res;
}

int getEndIndexBracket(char *line, int bracketPos)
{
    int depth = 0;
    int i;
    for (i = bracketPos + 1; depth != 0 || line[i] != ')'; i++)
        if (line[i] == '(')
            depth++;
        else if(line[i] == ')')
            depth--;
    return i;
}

