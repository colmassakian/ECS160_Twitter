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
	
	if(length >= SIZE)
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
char* find_delimiter(char* string, char const * delimiter){
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

int num_cols(char* s) 
{ 
    // Count variable 
    int res = 0; 
  
    for (int i = 0; i < strlen(s); i ++) 
    {
        if (s[i] == ',') 
            res ++; 
    }
  
    return res + 1; 
}

// Returns -1 if a duplicate string is found
int find_dups(char** columns, int num_cols)
{
	for(int i = 0; i < num_cols - 1; i ++)
	{
		for(int j = i + 1; j < num_cols; j ++)
		{
			if(strlen(columns[i]) == strlen(columns[j]) && !strncmp(columns[i], columns[j], strlen(columns[i])))
				return -1;
		}
	}

	// Free duplicate checking array
	for(int i = 0; i < num_cols; i ++)
		free(columns[i]);
	free(columns);

	return num_cols;
}

// Check if header uses quotes correctly and does not have duplicates
int check_line(char* line, int num_cols, int usingQuotes)
{
	char* tok;
	int count = 0;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(tok[0] == '"' && tok[strlen(tok) - 1] != '"')
        	return -1;
        
	    if(tok[0] != '"' && tok[strlen(tok) - 1] == '"')
	        return -1;

	    if(!usingQuotes && tok[0] == '"' && tok[strlen(tok) - 1] == '"')
	        return -1;

	    if(usingQuotes && tok[0] != '"' && tok[strlen(tok) - 1] != '"')
	        return -1;

	    count ++;
	}

	if(count != num_cols + 1)
	    return -1;

	return 1;
}

// Check if header uses quotes correctly and does not have duplicates
int check_header(char* line, int* usingQuotes)
{
	const char* tok;
	int count = 0;
	int quotes = 0;
	int noQuotes = 0;
	int cols = num_cols(line);

	char** col_names = (char**) malloc(cols * sizeof(char*));
	if(col_names == NULL)
		return -1;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(tok[0] == '"' && tok[strlen(tok) - 1] != '"')
        	return -1;
        
	    if(tok[0] != '"' && tok[strlen(tok) - 1] == '"')
	        return -1;

	    if(tok[0] == '"' && tok[strlen(tok) - 1] == '"')
	        quotes = 1;

	    if(tok[0] != '"' && tok[strlen(tok) - 1] != '"')
	        noQuotes = 1;

	    if(quotes && noQuotes)
	    	return -1;

	    col_names[count] = (char*)malloc(strlen(tok) * sizeof(char));
	    if(col_names[count] == NULL)
	    	return -1;
	    strcpy(col_names[count], tok);
	    count ++;
	}

	*usingQuotes = quotes;
	return find_dups(col_names, cols);
}

// Find the column in the csv that has the value "name"
int find_name(char* line, int usingQuotes)
{
	const char* tok;
	int count = 1;
	
	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(usingQuotes == 0 && strlen(tok) == strlen("name") && !strncmp(tok, "name", strlen(tok)))
			return count;
		if(usingQuotes == 1 && strlen(tok) == strlen("\"name\"") && !strncmp(tok, "\"name\"", strlen(tok)))
			return count;
		count ++;
	}
	return -1;
}


// Get the text from the specified column in the csv
char* get_field(char* line, int num, int usingQuotes)
{
	char* tok;

	// Num is <= 0
	if(!num)
		return NULL;

	for(tok = find_delimiter(line, ","); tok; tok = find_delimiter(NULL, ",\n"))
	{
		if(!--num)
	    {
            // Remove quotes from string
	        if(usingQuotes)
	        {
	            tok ++;
	            tok[strlen(tok) - 1] = 0;
	        }

	        return tok;
	    }
	}
	return NULL;
}

int main(int argc, char* argv [])
{
	if(argc != 2)
	{
		printf("Invalid input format\n");
		return -1;
	}
		
	FILE* stream = fopen(argv[1], "r");

	if(stream == NULL)
	{
		printf("Can't open file\n");
		return -1;
	}
	
	char line[LENGTH];
	char* tmp;
	char* tmp_header;
	char* name;
	char* tmp_name;
	int name_check;
	int num_cols = 0;
	int usingQuotes = 0;
	int name_column = 0;
	
	// Read first line to get column number for "name"
	if(fgets(line, LENGTH, stream))
	{
		tmp = strdup(line);
		tmp_header = strdup(line);
		num_cols = check_header(tmp_header, &usingQuotes);
		if(num_cols == -1)
		{
			printf("Invalid header format\n");
			return -1;
		}
		name_column = find_name(tmp, usingQuotes);
		length ++;
	}
	else
	{
		printf("Can't read header\n");
		return -1;
	}

	while (fgets(line, LENGTH, stream))
	{
		tmp = strdup(line);
		tmp_name = strdup(line);
		name_check = check_line(tmp_name, num_cols, usingQuotes);
		if(name_check == -1)
		{
			printf("Invalid line format\n");
			return -1;
		}
		name = get_field(tmp, name_column, usingQuotes);
		
		if(name == NULL)
		{
			printf("Can't find name\n");
			return -1;
		}

		if(insert(name) == -1)
		{
			printf("Can't insert name\n");
			return -1;
		}

		length ++;
		free(tmp);
	}

	get_top_n(10);
	destroy_hash();

	return 0;
}
