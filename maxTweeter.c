#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 20000
#define LENGTH 1024

typedef struct NameCountPair {
	char* name;
	int count;
} NameCountPair;

char *find_delimiter(char * string, char const * delimiter){
    static char *source = NULL;
    char *p, *res = 0;

    if(string != NULL)
        source = string;
    if(source == NULL)
        return NULL;

    if((p = strpbrk (source, delimiter)) != NULL) {
        *p  = 0;
        res = source;
        source = ++p;
    }
    else
    {
        res = source;
        source = NULL;
    }
        
    return res;
}

int find_name(char* line)
{
	const char* tok;
	int count = 1;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(!strcmp(tok, "name"))
			return count;
		count ++;
	}
	return -1;
}

char* get_field(char* line, int num)
{
	char* tok;

	// Num is <= 0
	if(!num)
		return NULL;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(!--num)
			return tok;
	}
	return NULL;
}

int main()
{
	FILE* stream = fopen("cl-tweets-short.csv", "r");

	if(stream == NULL)
	{
		printf("Invalid Input Format\n");
		return -1;
	}

	char line[LENGTH];
	char* tmp;
	char* name;
	int name_column = 0;

	if(fgets(line, LENGTH, stream))
	{
		tmp = strdup(line);
		name_column = find_name(tmp);
	}
	else
	{
		printf("Invalid Input Format\n");
		return -1;
	}

	while (fgets(line, LENGTH, stream))
	{
		tmp = strdup(line);
		name = get_field(tmp, name_column);
		
		if(name == NULL)
		{
			printf("Invalid Input Format\n");
			return -1;
		}
		printf("Name is %s\n", name);
		
		free(tmp);
	}

	return 0;
}
