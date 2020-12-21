#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Allergenes: wheat, shellfisch, soy, dairy, peanuts, nuts, eggs, sesame
#define ALLERGENE_COUNT 8
#define ALLERGENE_LEN 9
#define FOOD_COUNT 33
#define MAX_ALLERGENE_COUNT 3
#define MAX_INGREDIENT_COUNT 90
#define INGREDIENT_LEN 8

typedef struct Allergene{
    int hash;
    char *name;
} Allergene;

typedef struct Food{
    char **ingredients;
    Allergene *allergenes; 
} Food;

size_t iterations = 0;
void printAllergenes(FILE *in);
int hash(char *allergene);
Food *parseFoods(FILE *in);
char **getIngredients(char *ingredients);
Allergene *getAllergenes(char *allergenes);
char ***getAllergenesToIngredients(Food *food);
char **intersect(char **a, char **b);
int contains(char **haystack, char *needle);
char **deduplicate(char ***all);
int count(char **set);
char *first(char **set);
void removeAll(char ***all, char *toRemove);
int countNonAllergic(Food *food, char **allergic);

int main(int argc, char *argv[])
{
	FILE *in = fopen("in21", "r");

    Food *food = parseFoods(in);
    char ***allergenesToIngredients = getAllergenesToIngredients(food);
    char **deduplicated = deduplicate(allergenesToIngredients);
    //for (int i = 0; i < ALLERGENE_COUNT; i++)
        //printf("Allergene %d is in %s\n", i, deduplicated[i]);
    printf("Non allergic: %d\n", countNonAllergic(food, deduplicated));
    printf("Canonical dangerous ingredient list:\n");
    for (int i = 0; i < ALLERGENE_COUNT; i++)
        printf("%s%s", deduplicated[i], i < ALLERGENE_COUNT - 1 ? "," : "");
    printf("\n");

    for (int i = 0; i < FOOD_COUNT; i++)
    {
        for (int j = 0; j < MAX_INGREDIENT_COUNT; j++)
            free(food[i].ingredients[j]);
        for (int j = 0; j < MAX_ALLERGENE_COUNT; j++)
            free(food[i].allergenes[j].name);

        free(food[i].ingredients);
        free(food[i].allergenes);
    }
    for (int i = 0; i < ALLERGENE_COUNT; i++)
        free(allergenesToIngredients[i]);
    free(deduplicated);
    
    free(allergenesToIngredients);
    free(food);
	fclose(in);	
	return 0;	
}

int countNonAllergic(Food *food, char **allergic)
{
    int res = 0;
    for (int i = 0; i < FOOD_COUNT; i++)
    {
        for (int j = 0; j < MAX_INGREDIENT_COUNT; j++)
        {
            if (food[i].ingredients[j] == NULL)
                break;
            for (int k = 0; k < ALLERGENE_COUNT; k++)
                if (!strcmp(food[i].ingredients[j], allergic[k]))
                {
                    res--;
                    break;
                }
            res++;
        }
    }
    return res;
}

int count(char **set)
{
    int count = 0;
    for (int i = 0; i < ALLERGENE_COUNT; i++)
        if (set[i] != NULL)
            count++;
    return count;
}

char *first(char **set)
{
    for (int i = 0; i < ALLERGENE_COUNT; i++)
        if (set[i] != NULL)
            return set[i];
    return NULL;
}

void removeAll(char ***all, char *toRemove)
{
    for (int i = 0; i < ALLERGENE_COUNT; i++)
    {
        if (all[i] == NULL)
            continue;
        for (int j = 0; j < ALLERGENE_COUNT; j++)
            if (all[i][j] != NULL && !strcmp(all[i][j], toRemove))
                all[i][j] = NULL;
    }
}

char **deduplicate(char ***all)
{
    char **res = calloc(ALLERGENE_COUNT, sizeof(*res));
    for (int i = 0; i < ALLERGENE_COUNT;)
    {
        for (int j = 0; j < ALLERGENE_COUNT; j++)
        {
            if (res[j] != NULL)
                // Already known
                continue;
            if (count(all[j]) == 1)
            {
                i++;
                res[j] = first(all[j]);;
                removeAll(all, res[j]);
                break;
            }
        }
    }
    return res;
}

char ***getAllergenesToIngredients(Food *food)
{
    char ***res = calloc(ALLERGENE_COUNT, sizeof(*res));
    for (int i = 0; i < ALLERGENE_COUNT; i++)
        res[i] = calloc(MAX_INGREDIENT_COUNT, sizeof(*res[i]));
    for (int i = 0; i < FOOD_COUNT; i++)
        for (int j = 0; j < MAX_ALLERGENE_COUNT; j++)
        {
            if (food[i].allergenes[j].name == NULL)
                // No more allergenes
                break;
            int h = food[i].allergenes[j].hash;
            if (res[h][0] == NULL)
            {
                // No ingredients assigned yet
                size_t size = MAX_INGREDIENT_COUNT * sizeof(*res[h]);
                memcpy(res[h], food[i].ingredients, size);
            }
            else
            {
                // Some ingredients assigned already - get intersection
                char **new = intersect(res[h], food[i].ingredients);
                free(res[h]);
                res[h] = new;
            }
        }
    return res;
}

char **intersect(char **a, char **b)
{
    char **res = calloc(MAX_INGREDIENT_COUNT, sizeof(*res));
    int j = 0;
    for (int i = 0; i < MAX_INGREDIENT_COUNT; i++)
    {
        if (a[i] == NULL)
            break;
        if (contains(b, a[i]))
            res[j++] = a[i];
    }
    return res;
}

int contains(char **haystack, char *needle)
{
    for (int i = 0; i < MAX_INGREDIENT_COUNT; i++)
    {
        if (haystack[i] == NULL)
            return 0;
        if (!strcmp(haystack[i], needle))
            return i+1;
    }
    return 0;
}

int hash(char *allergene)
{
    if (!strcmp("wheat", allergene))
        return 7;
    if (!strcmp("shellfish", allergene))
        return 5;
    if (!strcmp("soy", allergene))
        return 6;
    if (!strcmp("dairy", allergene))
        return 0;
    if (!strcmp("peanuts", allergene))
        return 3;
    if (!strcmp("nuts", allergene))
        return 2;
    if (!strcmp("eggs", allergene))
        return 1;
    if (!strcmp("sesame", allergene))
        return 4;
    fprintf(stderr, "PANIC\n");
    return -1;
}

Food *parseFoods(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    Food *res = calloc(FOOD_COUNT, sizeof(*res));
    int i = 0;
    while((getline(&line, &n, in)) > 0 && *line != '\0' && *line != '\n')
    {
        char *ptr = strchr(line, '(');
        if (ptr == NULL)
        {
            fprintf(stderr, "PANIC\n");
            free(line);
            return NULL;
        }
        *ptr = '\0';
        ptr += strlen("(contains");
        *strchr(ptr, ')') = '\0';
        res[i].ingredients = getIngredients(line);
        res[i++].allergenes = getAllergenes(ptr);
    }
    free(line);
    return res;
}

char **getIngredients(char *ingredients)
{
    char *token;
    char **res = calloc(MAX_INGREDIENT_COUNT, sizeof(*res));
    int i = 0;
    while ((token = strtok(ingredients, " ")) != NULL)
    {
        res[i++] = strdup(token);
        ingredients = NULL;
    }
    return res;
}

Allergene *getAllergenes(char *allergenes)
{
    char *token;
    Allergene *res = calloc(MAX_ALLERGENE_COUNT, sizeof(*res));
    int i = 0;
    while ((token = strtok(allergenes, ",")) != NULL)
    {
        res[i].name = strdup(++token);
        res[i++].hash = hash(token);
        allergenes = NULL;
    }
    return res;
}

void printAllergenes(FILE *in)
{
    size_t n = 0;
    char *line = NULL;
    while((getline(&line, &n, in)) > 0 && *line != '\0' && *line != '\n')
    {
        char *ptr = strstr(line, "contains");
        if (ptr == NULL)
        {
            fprintf(stderr, "PANIC\n");
            free(line);
            return;
        }
        ptr += strlen("contains");
        *strchr(ptr, ')') = '\0';
        printf("Got allergenes%s\n", ptr);
    }
    free(line);
}

