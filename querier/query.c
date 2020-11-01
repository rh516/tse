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
#include <indexio.h>

bool isValid(char* c){
	char *curr = c;
	while ((int)(*curr) != 10) { 
		if (!isalpha(*curr)) {
			if (!((int)(*curr) == 9 || (int)(*curr) == 32)) {
				return false;
			}
		}
		curr += 1;
	}
	return true;
}


void NormalizeWord(char *word) {
	for (char *alpha = word; *alpha != '\0'; alpha++) {
		*alpha = tolower(*alpha);
	}
}


bool wordSearch(void *word, const void *wordKey) {
	if (word != NULL && wordKey != NULL) {
		char *w = (char *) wordKey;
		wordDocQueue_t *wdq = (wordDocQueue_t *) word;

		if (strcmp(w, wdq->word) == 0) {
			return true;
		}
	}
	return false;
}


bool docSearch(void *doc, const void *id) {
	int *docID = (int *)id;
	docCount_t *dp = (docCount_t *) doc;

	if (docID != NULL && doc != NULL) {
		if (dp->id == *docID) {
			return true;
		}
	}
	return false;
}

void freeQ(void *ep) {
	wordDocQueue_t *temp = (wordDocQueue_t *)ep;
	free(temp->word);
	qclose(temp->qp);
}

int main(void) {
  char input[101];
  hashtable_t *index = indexload("../indexer/index1");

  printf("> ");
  while (fgets(input, 101, stdin) != NULL) {
    if (!isValid(input)) {
      printf("Invalid query \n");
    }
    else {
      NormalizeWord(input);
      input[strlen(input) - 1] = '\0';
      
      char *token = strtok(input, " ");
      char *wordArr[50];
      int i = 0;

      while(token != NULL) {
        wordArr[i++] = token;
        token = strtok(NULL, " ");
      }

      int k = 0;
      int32_t minCount = 1000000;

      while (k < i) {
        if (!(strcmp(wordArr[k], "and") == 0 || strcmp(wordArr[k], "or") == 0 || strlen(wordArr[k]) < 3)) {
          wordDocQueue_t *foundWord;
          docCount_t *foundDoc;
          int id = 1;

          if ((foundWord = hsearch(index, wordSearch, wordArr[k], strlen(wordArr[k])))) {
            if ((foundDoc = qsearch(foundWord -> qp, docSearch, &id))) {
              if (foundDoc -> count < minCount) {
                minCount = foundDoc -> count;
              } 
              printf("%s: %d\n", foundWord -> word, foundDoc -> count);
            }
          }          
        }
        k++;
      }
      printf("Min Count: %d\n", minCount);
    }
    printf("> ");
  }

  happly(index, freeQ);
  hclose(index);
  printf("\n");
  return 0;
}