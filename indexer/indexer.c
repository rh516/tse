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

typedef struct docQueue {
	queue_t *qp;
	char *word;
} docQueue_t;


typedef struct document {
	int id;
	int count;
} document_t;


void NormalizeWord(char *word) {
	for (char *alpha = word; *alpha != '\0'; alpha++) {
		*alpha = tolower(*alpha);
	}
}

bool doc_wordSearch(void *doc, const void *d) {
	int *docID = (int *)d;
	document_t *dp = (document_t *) doc;
	if (docID != NULL && doc != NULL) {
		if (dp->id == *docID) {
			return true;
		}
	}
	return false;
}

bool doc_qsearch(void *dq, const void *word) {
	if (dq != NULL && word != NULL) {
		char *w = (char *) word;
		docQueue_t *docsq = (docQueue_t *) dq;
		if (strcmp(w, docsq->word) == 0) {
			return true;
		}
	}
	return false;
}

void freeQ(void *ep) {
	docQueue_t *temp = (docQueue_t *)ep;
	free(temp->word);
	qclose(temp->qp);
}


int totalCount = 0;
void qsum(void *ep) {
	document_t *dp = (document_t *) ep;
	totalCount += dp->count;
}

void finalCountQ(void *elementPtr) {
	docQueue_t *temp = (docQueue_t *)elementPtr;
	qapply(temp->qp, qsum);
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
				docQueue_t *tempq;
            if ((tempq = hsearch(index, doc_qsearch, word, strlen(word))) == NULL) {
								docQueue_t *docsq = malloc(sizeof(docQueue_t));

								docsq->word = word;
								docsq->qp = qopen();

								document_t *doc = malloc(sizeof(document_t));
								doc->id = idx;
								doc->count = 1;

								qput(docsq->qp, doc);
								hput(index, docsq, word, strlen(word));
            }
            else {
							free(word);
							document_t *tempdoc;
							if ((tempdoc = qsearch(tempq->qp, doc_wordSearch, &idx)) == NULL) {
								document_t *doc = malloc(sizeof(document_t));
								doc->id = idx;
								doc->count = 1;
								qput(tempq->qp, doc);
							}
							else {
								tempdoc->count++;
							}
						}
				}
			else {
				free(word);
			}
		}
		webpage_delete(page);
	}
	happly(index, finalCountQ);
	printf("Total Count: %d\n", totalCount);

	happly(index, freeQ);
	fclose(newFile);
	hclose(index);

	return 0;
}
