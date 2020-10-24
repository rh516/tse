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


typedef struct wordStruct
{
    char *word;
    int count;
} wordStruct_t;


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


wordStruct_t *makeWordStruct(char *word) 
{   
    if (!word) {
        return NULL;
    }

    wordStruct_t *newWordStruct;

    if (!(newWordStruct = (wordStruct_t *)malloc(sizeof(wordStruct_t))))
    {
        printf("Error: malloc failed allocating wordStruct\n");
        return NULL;
    }

    if (!(newWordStruct -> word = (char *)malloc(strlen(word) + 1)))
    {
        printf("Error: malloc failed allocating word\n");
        return NULL;
    }

    strcpy(newWordStruct -> word, word);
    newWordStruct -> count = 1;

    return newWordStruct;
}


bool search(void *elementPtr, const void *keyPtr)
{
	wordStruct_t *wStruct = (wordStruct_t *)elementPtr;
	char *key = (char *)keyPtr;

	return strcmp(key, wStruct -> word) == 0;
}


void freeWord(void *elementPtr)
{
    wordStruct_t *wStruct = (wordStruct_t *)elementPtr;
    free(wStruct -> word);
}


void printInfo(void *elementPtr) 
{
    wordStruct_t *wStruct = (wordStruct_t *) elementPtr;
    printf("Word: %s, Count: %d\n", wStruct -> word, wStruct -> count);
}


int totalCount = 0;
void incrementCount(void *elementPtr)
{
    wordStruct_t *wStruct = (wordStruct_t *)elementPtr;
    totalCount += wStruct -> count;
}


int main(void) 
{
    webpage_t *page = pageload(1, "../crawler/pages");
    FILE *newFile = fopen("output1", "w");

    int pos = 0;
	char *word;
    hashtable_t *index = hopen(50);

	while ((pos = webpage_getNextWord(page, pos, &word)) > 0)
    {
        if (strlen(word) >= 3) 
        {   
            NormalizeWord(word);

            if (!hsearch(index, search, word, strlen(word)))
            {
                wordStruct_t *newWordStruct = makeWordStruct(word);
                hput(index, newWordStruct, newWordStruct -> word, strlen(newWordStruct -> word));
            }
            else 
            {
                wordStruct_t *foundWordStruct = hsearch(index, search, word, strlen(word));
                (foundWordStruct -> count)++;
            }

            fprintf(newFile, "%s\n", word);
        }
        
        free(word); 
    }

    happly(index, printInfo);
    happly(index, incrementCount);
    printf("Total Count: %d\n", totalCount);

    fclose(newFile);
    webpage_delete(page);
    happly(index, freeWord);
    hclose(index);
    
    return 0;
}