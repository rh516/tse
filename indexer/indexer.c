#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <sys/stat.h>
#include <pageio.h>
#include <ctype.h>


void NormalizeWord(char *word) 
{
    if (word) 
    {
        for (char *c = word; *c; c++)
        {
            *c = tolower(*c);
        }
    }
}


int main(void) 
{
    webpage_t *page = pageload(1, "../crawler/pages");
    FILE *newFile = fopen("output1", "w");

    int pos = 0;
	char *word;
	while ((pos = webpage_getNextWord(page, pos, &word)) > 0)
    {
        if (strlen(word) > 3) 
        {   
            NormalizeWord(word);
            fprintf(newFile, "%s\n", word);
        }
        free(word);
    }
    fclose(newFile);
    webpage_delete(page);
    
    return 0;
}
