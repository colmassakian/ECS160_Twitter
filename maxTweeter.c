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

const char* get_field(char* line, int num)
{
	const char* tok;

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

	char line[LENGTH];
	int name_column = 0;

	if(fgets(line, LENGTH, stream))
	{
		char* tmp = strdup(line);
		name_column = find_name(tmp);
	}
	else
		return -1;

	printf("COUNT: %d\n", name_column);
	while (fgets(line, LENGTH, stream))
	{
		char* tmp = strdup(line);
		printf("Field would be %s\n", get_field(tmp, name_column));
		// NOTE strtok clobbers tmp
		free(tmp);
	}

	return 0;
}
