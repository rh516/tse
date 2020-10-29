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


void NormalizeWord(char *word) {
	for (char *alpha = word; *alpha != '\0'; alpha++) {
		*alpha = tolower(*alpha);
	}
}


// searches for doc
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


// searches for word
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


int totalCount = 0;
void incrementCount(void *ep) {
	docCount_t *dp = (docCount_t *) ep;
	totalCount += dp->count;
}


void sumQ(void *elementPtr) {
	wordDocQueue_t *temp = (wordDocQueue_t *)elementPtr;
	qapply(temp->qp, incrementCount);
}


bool dir_exists(char *dirname) {
	static const char crawlerfile[] = ".crawler";
	int filenamelength = strlen(dirname) + strlen(crawlerfile) + 2;
	char *file = malloc(filenamelength);

	sprintf(file, "%s/%s", dirname, crawlerfile);
	
  FILE *fp = fopen(file, "w");                                     
  if (fp == NULL) {
		free(file);
		return false;                                                               
	}
	else {
		fclose(fp);
		free(file);
		return true;
	}
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
		printf("usage: indexer <id>\n");
		return 1;
		}

		
		char *dir = argv[1];
		char *indexnm = argv[2];

    //check if directory exists
		if (!dir_exists(dir)) {
			printf("directory does not exist\n");
			return 1;
		}
		
    FILE *newFile = fopen("output1", "w");
    hashtable_t *index = hopen(100);
		int idx = 1;

		webpage_t *page = pageload(idx, dir);
		while (idx < 7) {
			int pos = 0;
			char *word;
        
			while ((pos = webpage_getNextWord(page,pos,&word)) > 0) {
				if (strlen(word) >= 3) { 
					NormalizeWord(word);

					wordDocQueue_t *foundWord;
					if (!(foundWord = hsearch(index, wordSearch, word, strlen(word)))) {
						// wordDocQueue doesn't exist yet, so create it
						wordDocQueue_t *wdq = makeWordDocQueue(word);
						docCount_t *docCount = makeDocCount(idx, 1);

						qput(wdq -> qp, docCount);
						hput(index, wdq, word, strlen(word));
					}
					else {
						// wordDocQueue already exists
						free(word);
						docCount_t *foundDoc;
						if (!(foundDoc = qsearch(foundWord->qp, docSearch, &idx))) {
						// doc hasn't been added to queue yet, so add it
							docCount_t *doc = makeDocCount(idx, 1);						
							qput(foundWord -> qp, doc);
						}
						else {
							// doc is already there, increment count
							(foundDoc -> count)++;
						}
					}
				}
				else {
					free(word);
				}
			}
			webpage_delete(page);
			idx++;
		  page = pageload(idx, dir);
		}

		webpage_delete(page);
		happly(index, sumQ);
		printf("Total Count: %d\n", totalCount);

		indexsave(index, indexnm);
		happly(index, freeQ);

		fclose(newFile);
		hclose(index);

		return 0;
}
