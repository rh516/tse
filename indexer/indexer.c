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


int main(int argc, char *argv[]) {
	 int id = atoi(argv[1]);
	 FILE *newFile = fopen("output1", "w");
	 hashtable_t *index = hopen(50);
	
	for(int idx = 1; idx <= id; idx++) {
		webpage_t *page = pageload(idx, "../crawler/pages");
		int pos = 0;
		char *word;
		
		while ((pos = webpage_getNextWord(page,pos,&word)) > 0) {
			if (strlen(word) >= 3) { 
				NormalizeWord(word);
				wordDocQueue_t *tempq;
				if ((tempq = hsearch(index, wordSearch, word, strlen(word))) == NULL) {
                    // // wordDocQueue doesn't exist yet, so create it
                    // wordDocQueue_t *docsq = malloc(sizeof(wordDocQueue_t));

                    // docsq->word = word;
                    // docsq->qp = qopen();
                    wordDocQueue_t *docsq = makeWordDocQueue(word);

                    docCount_t *doc = makeDocCount(idx, 1);
                    // docCount_t *doc = malloc(sizeof(docCount_t));
                    // doc->id = idx;
                    // doc->count = 1;

                    qput(docsq->qp, doc);
                    hput(index, docsq, word, strlen(word));
				}
				else {
					// wordDocQueue already exists
					free(word);
					docCount_t *tempdoc;
					if ((tempdoc = qsearch(tempq->qp, docSearch, &idx)) == NULL) {
						// // doc hasn't been added to queue yet, so add it
						// docCount_t *doc = malloc(sizeof(docCount_t));
						// doc->id = idx;
						// doc->count = 1;
						docCount_t *doc = makeDocCount(idx, 1);
						
						qput(tempq->qp, doc);
					}
					else {
						// doc is already there, increment count
						(tempdoc->count)++;
					}
				}
			}
			else {
				free(word);
			}
		}
		webpage_delete(page);
	}
	happly(index, sumQ);
	printf("Total Count: %d\n", totalCount);

	indexsave(index, "output1");
	
	happly(index, freeQ);
	fclose(newFile);
	hclose(index);

	return 0;
}
