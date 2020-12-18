#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t iterations = 0;
long solution(FILE *in);
// Start inclusive, end exclusive
long calculate(char *line, int start, int end);
int getEndIndexBracket(char *line, int bracketPos);
long apply(int op, long curr, long atom);

int PART2 = 0;

int main()
{
	FILE *in = fopen("in18", "r");

    long res = solution(in);
    printf("Part 1: %ld\n", res);
    PART2 = 1;
    rewind(in);
    res = solution(in);
    printf("Part 2: %ld\n", res);

	fclose(in);	
	return 0;	
}

long solution(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    long res = 0;
    while (getline(&line, &n, in) > 0 && *line != '\n' && *line != '\0')
        res += calculate(line, 0, strlen(line));
    free(line);
    return res;
}

long calculate(char *line, int start, int end)
{
    long res = 0;
    // 0 -> Nothing, 1 -> Add, 2 -> Multiply
    int lastOperator = 0;
    for (int i = start; i < end; i++)
    {
        long atom = -1;
        int endIndex;
        switch(line[i])
        {
            case ' ':
            case '\n':
                // Separates atoms
                continue;
            case '+':
                lastOperator = 1;
                continue;
            case '*':
                if (PART2)
                {
                    atom = calculate(line, i+1, end);
                    return apply(2, res, atom);
                }
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
                atom = line[i] - '0';
                break;
        }

        res = apply(lastOperator, res, atom);
    }
    return res;
}

long apply(int op, long curr, long atom)
{
    switch(op)
    {
        case 0:
            return atom;
        case 1:
            return curr + atom;
        case 2:
            return curr * atom;
        default:
            fprintf(stderr, "Invalid operator: %d\n", op);
            return -1;
    }
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

