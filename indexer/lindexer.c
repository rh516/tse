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
#include <lhash.h>
#include <lqueue.h>
#include <pthread.h>

typedef struct arginfo {
	lhashtable_t *lindex;
	char *pagedir;
} arginfo_t;

void *makeindex(void *args);

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

void freeQ(void *ep) {
	wordDocQueue_t *temp = (wordDocQueue_t *)ep;
	free(temp->word);
	qclose(temp->qp);
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
	// check for valid arguments
	if (argc < 4) {
		printf("usage: indexer <pagedir> <indexnm> <numThreads>\n");
		return 1;
	}

	char *dir = argv[1];
	char *indexnm = argv[2];
	int numThreads = atoi(argv[3]);

	//open locked hashtable
	lhashtable_t *index = lhopen(1000);
	
	arginfo_t *args[numThreads];
	pthread_t thread[numThreads];
	
	//check if directory exists
	if (!dir_exists(dir)) {
		printf("directory does not exist\n");
		return 1;
	}

	//create threads and call makeindex
	for (int i = 1; i <= numThreads; i++) {
		args[i] = malloc(sizeof(arginfo_t *));
		args[i]->lindex = index;
		args[i]->pagedir = dir;
		pthread_create(&thread[i], NULL, makeindex, (void *)args[i]);
	}

	//join threads
	for (int j =1; j<=numThreads; j++) {
		pthread_join(thread[j], NULL);
	}

	// print total number of unique words
	lhapply(index, sumQ);
	printf("Total Count: %d\n", totalCount);

	//save index - modified indexsave to take in a locked hash table
	printf("saving index now\n");

	indexsave(index -> htptr, indexnm);
	
	// free all the queues in the index and then the index
	lhapply(index, freeQ);
	lhclose(index);
	return 0;
}

void *makeindex(void *args) {
	// get all the various arguments from the void argument passed in 
	arginfo_t *arg = (arginfo_t *)args;
	char *dir = arg->pagedir;
	webpage_t *page;
	int idx = 1;

	// keep going as long as there is no next page
	while ((page = pageload(idx, dir)) != NULL) {
		char *word;
		int pos = 0;

		// while the word is still found
		while ((pos = webpage_getNextWord(page,pos,&word)) > 0) {
			if (strlen(word) >= 3) { 
				NormalizeWord(word);
				
				wordDocQueue_t *foundWord;
				if (!(foundWord = lhsearch(arg->lindex, wordSearch, word, strlen(word)))) {
					// wordDocQueue doesn't exist yet, so create it
					wordDocQueue_t *wdq = makeWordDocQueue(word);
					docCount_t *docCount = makeDocCount(idx, 1);
					
					qput(wdq -> qp, docCount);
					lhput(arg->lindex, wdq, word, strlen(word));
					free(word);
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
		// free the page
		webpage_delete(page);
		idx ++;
	}
	return 0;
}
