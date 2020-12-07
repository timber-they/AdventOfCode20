#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAGS_COUNT 594
#define LINE_LENGTH 128
#define MAX_REFERENCES 10

struct Rule
{
	char *color;
	struct Rule **references;
};

struct Rule *createRules(char *lines[]);
char **getBags(char *lines[]);
void replace(char *text, char toReplace, char replacement);
void resolveReferences(struct Rule *bags, struct Rule *rule, char **references);
struct Rule *point (struct Rule *haystack, char *needle);
void printRule(struct Rule rule, int depth);
struct Rule **getTransitiveOrigins(struct Rule *rule, struct Rule *rules);
int contains(struct Rule **haystack, struct Rule *needle);

int main(void)
{
	FILE *in = fopen("in7", "r");

	char *lines[594];
	struct Rule *rules;
	struct Rule **origins;
	int res;

	for (int i = 0; i < BAGS_COUNT; i++)
	{
		size_t n = 0;
		getline(lines+i, &n, in);
	}

	rules = createRules(lines);
	//for (int i = 0; i < BAGS_COUNT; i++)
		//printRule(rules[i], 1);
	origins = getTransitiveOrigins(point(rules, "shiny gold"), rules);
	for (res = 0; origins[res] != NULL; res++)
		; // Ignored
	// One less, because a bag doesn't contain itself
	printf("Count: %d\n", res-1);

	fclose(in);
	return 0;	
}

struct Rule **getTransitiveOrigins(struct Rule *rule, struct Rule *rules)
{
	struct Rule **origins = calloc(BAGS_COUNT, sizeof(*origins));
	struct Rule **current = calloc(BAGS_COUNT, sizeof(*current));
	// The origin index
	int o = 0;
	current[0] = rule;

	// Iterate as long as we find new origins
	while(*current != NULL)
	{
		struct Rule **new = calloc(BAGS_COUNT, sizeof(*new));
		// The index of the new origins we found
		int l = 0;

		// For these bags we are currently searching the origin for
		for (int j = 0; current[j] != NULL && j < BAGS_COUNT; j++)
		{
			if (contains(origins, current[j]))
			{
				// No double searches
				current[j] = NULL;
				continue;
			}

			origins[o++] = current[j];

			// Each rule might be a new origin
			for (int i = 0; i < BAGS_COUNT; i++)
			{
				// We have to check each reference of possible origins
				for (int k = 0; rules[i].references[k] != NULL; k++)
				{
					// We found an origin
					if (rules[i].references[k] == current[j])
					{
						new[l++] = rules+i;
					}
				}
			}
			// We're done with that one
			current[j] = NULL;
		}

		for (int i = 0; new[i] != NULL; i++)
			current[i] = new[i];
	}

	return origins;
}

int contains(struct Rule **haystack, struct Rule *needle)
{
	for (int i = 0; haystack[i] != NULL; i++)
		if (haystack[i] == needle)
			return 1;
	return 0;
}

struct Rule *createRules(char *lines[])
{
	char **bags = getBags(lines);
	struct Rule *rules = calloc(BAGS_COUNT, sizeof(*rules));

	for (int i = 0; i < BAGS_COUNT; i++)
		rules[i].color = bags[i];

	for (int i = 0; i < BAGS_COUNT; i++)
	{
		char *token = strtok(lines[i], "123456789");
		char **references = calloc(MAX_REFERENCES, sizeof(*references));

		for (int j = 0; (token = strtok(NULL, "123456789")) != NULL; j++)
		{
			char *mod = calloc(LINE_LENGTH, sizeof(*mod));
			references[j] = calloc(LINE_LENGTH, sizeof(*references[j]));
			if (!sscanf(token, "%s %s", references[j], mod))
				fprintf(stderr, "Invalid token: %s\n", token);
			else
			{
				strcat(references[j], " ");
				strcat(references[j], mod);
			}
		}

		resolveReferences(rules, rules+i, references);	
	}
	return rules;
}

void printRuleIndent(struct Rule rule, int depth, int indent)
{
	printf("%s\n", rule.color);
	if (depth == 0)
		return;
	for (int i = 0; rule.references[i] != NULL; i++)
	{
		for (int i = 0; i < indent; i++)
			printf("    ");
		printf(" %s ", rule.references[i+1] == NULL ? "└" : "├");
		printRuleIndent(*rule.references[i], depth-1, indent+1);
	}
}

void printRule(struct Rule rule, int depth)
{
	printRuleIndent(rule, depth, 0);
}

void resolveReferences(struct Rule *bags, struct Rule *rule, char **references)
{
	rule->references = calloc(MAX_REFERENCES, sizeof(*(rule->references)));

	for (int i = 0; references[i] != NULL; i++)
		rule->references[i] = point(bags, references[i]);
}

struct Rule *point (struct Rule *haystack, char *needle)
{
	for (int i = 0; i < BAGS_COUNT; i++)
		if (!strcmp(haystack[i].color, needle))
			return haystack+i;
	return NULL;
}

char **getBags(char *lines[])
{
	char **bags = calloc(BAGS_COUNT, sizeof(*bags));
	char *mod = calloc(LINE_LENGTH, sizeof(*mod));
	for (int i = 0; i < BAGS_COUNT; i++)
	{
		bags[i] = calloc(LINE_LENGTH, sizeof(*(bags[i])));
		if (!sscanf(lines[i], "%s %s bags contain%*s\n", bags[i], mod))
			fprintf(stderr, "Invalid line (resulted in %s): %s", bags[i], lines[i]);
		else
		{
			strcat(bags[i], " ");
			strcat(bags[i], mod);
		}
	}

	free(mod);

	return bags;
}

void replace(char *text, char toReplace, char replacement)
{
	char *pos;
	while ((pos = strchr(text, toReplace)) != NULL)
		*pos = replacement;
}
