#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 20000
#define LENGTH 1024

typedef struct NameCountPair {
	char* name;
	int count;
} NameCountPair;

NameCountPair tweeter_hash[SIZE];
int length;

int comparator(const void* p1, const void* p2)
{
   int first = ((struct NameCountPair *)p1)->count;
   int second = ((struct NameCountPair *)p2)->count;
   return second - first;
}

void destroy_hash()
{
	for(int i = 0; i < SIZE; i ++)
		free(tweeter_hash[i].name);
}

// Sort the hash table and print the top n results
void get_top_n(int n)
{
	qsort((void*)tweeter_hash, sizeof(tweeter_hash) / sizeof(tweeter_hash[0]), sizeof(tweeter_hash[0]), comparator);

	for(int i = 0; i < n; i ++)
	{
		printf("Name: %s, Count: %d\n", tweeter_hash[i].name, tweeter_hash[i].count);
	}
}

// Hash function for insertion in the hash table
int hash(char* input) { 
    int hash = 7;

	for (int i = 0; i < strlen(input); i++)
	    hash = hash * 31 + input[i];

	if(hash < 0)
		hash *= -1;

	return hash;
}

// If the key has not been inserted before, enter it with a value of 1
// If the key is already present, increment its value
int insert(char* key)
{
	int hashIndex = hash(key);
	if(hashIndex < 0)
		hashIndex *= -1;

	hashIndex %= SIZE;
	
	if(length > SIZE)
		return -1;
	
	while(tweeter_hash[hashIndex].name != NULL)
	{
		// Key has been inserted previously
		if(strlen(tweeter_hash[hashIndex].name) == strlen(key) && strncmp(tweeter_hash[hashIndex].name, key, strlen(key)) == 0)
		{
			tweeter_hash[hashIndex].count ++;
			return tweeter_hash[hashIndex].count;
		}
		
		++hashIndex;

		//wrap around the table
		hashIndex %= SIZE;
	}
	// Key was not found, this is the first tweet by tweeter
	tweeter_hash[hashIndex].name = (char*) malloc(strlen(key) * sizeof(char));
	if(tweeter_hash[hashIndex].name != NULL) {
		strcpy(tweeter_hash[hashIndex].name, key);
		tweeter_hash[hashIndex].count = 1;
	}
   	else
   		return -1;

   return tweeter_hash[hashIndex].count;
}

// Slight modified version of strtok to treat consecutive 
// appearances of the delimiter as separate
// ie strtok treats name,,text as two entries, but we need
// it to be treated as three, where the middle is empty
char *find_delimiter(char * string, char const * delimiter){
    static char *source = NULL;
    char *p, *res = 0;

    if(string != NULL)
        source = string;
    if(source == NULL)
        return NULL;

    if((p = strpbrk (source, delimiter)) != NULL)
    {
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

// Find the column in the csv that has the value "name"
int find_name(char* line)
{
	const char* tok;
	int count = 1;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(strlen(tok) == strlen("name") && !strncmp(tok, "name", strlen(tok)))
			return count;
		count ++;
	}
	return -1;
}


// Get the text from the specified column in the csv
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

int main(int argc, char* argv [])
{
	if(argc != 2)
	{
		printf("Invalid Input Format\n");
		return -1;
	}
		
	FILE* stream = fopen(argv[1], "r");

	if(stream == NULL)
	{
		printf("Invalid Input Format\n");
		return -1;
	}
	
	char line[LENGTH];
	char* tmp;
	char* name;
	int name_column = 0;
	
	// Read first line to get column number for "name"
	if(fgets(line, LENGTH, stream))
	{
		tmp = strdup(line);
		name_column = find_name(tmp);
		length ++;
	}
	else
	{
		printf("Invalid Input Format\n");
		return -1;
	}

	while (fgets(line, LENGTH, stream))
	{
		length ++;
		tmp = strdup(line);
		name = get_field(tmp, name_column);
		
		if(name == NULL)
		{
			printf("Invalid Input Format\n");
			return -1;
		}

		if(insert(name) == -1)
		{
			printf("Invalid Input Format\n");
			return -1;
		}
		
		free(tmp);
	}

	get_top_n(10);
	destroy_hash();

	return 0;
}
