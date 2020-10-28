/* indexio.c --- 
 * 
 * Description: indexio module to save and load index to inxdexnm file 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <hash.h>
#include <queue.h>
#include <indexio.h>

static FILE *f;

static void saveCount(void *element) {
	if (!element) {
			printf("null element\n");
		}
	else {
		docCount_t *tempdoc = (docCount_t *)element;
		fprintf(f, "%d %d ", tempdoc->id, tempdoc->count);
	}
}


static void saveLine(void *element) {
	if (!f || !element) {
		printf("null file or null wordDocQueue\n");
	}
	else {
		wordDocQueue_t *uniqueWord = (wordDocQueue_t *)element;

		fprintf(f, "%s ", uniqueWord->word);
		qapply(uniqueWord->qp, saveCount);
		fprintf(f, "\n");
	}
}


docCount_t *makeDocCount(int id, int count) {
	docCount_t *doc;

	if (!(doc = (docCount_t*)malloc(sizeof(docCount_t)))) {
		printf("Error: malloc failed allocating doc\n");
		return NULL;
	}

	doc -> id = id;
	doc -> count = count;
	return doc;	
}


wordDocQueue_t *makeWordDocQueue(char *word) {
	wordDocQueue_t *wdq;

	if (!(wdq = (wordDocQueue_t*)malloc(sizeof(wordDocQueue_t)))) { 
		printf("Error: malloc failed allocating wordDocQueue\n");
		return NULL;
	}

	if (!(wdq -> word = (char *)malloc((strlen(word) + 1) * sizeof(char)))) {
		printf("Error: malloc failed allocating word\n");
		return NULL;
	}

	strcpy(wdq -> word, word);
	wdq -> qp = qopen();

	return wdq;
}


void freeQ(void *ep) {
	wordDocQueue_t *temp = (wordDocQueue_t *)ep;
	free(temp->word);
	qclose(temp->qp);
}


int32_t indexsave(hashtable_t *index, char *fname) {
	f = fopen(fname, "w");

	if (!index || !fname) {
		printf("failed to open file");
		return 1;
	}

	happly(index, saveLine);
	fclose(f);
	
	return 0;
}


hashtable_t *indexload(char *fname) {
	if (!fname) {
		printf("indexload failed, cannot open file\n");
		return NULL;
	}

	f = fopen(fname, "r");

	hashtable_t *index = hopen(100);

	char *word = (char *) malloc(sizeof(char) * 51);
	int fScanVal = 0, id, count;

	while (fScanVal != EOF) {
		fscanf(f, "%s", word);
		wordDocQueue_t *wdq= makeWordDocQueue(word);
		hput(index, wdq, word, strlen(word));

		while ((fScanVal = fscanf(f, "%d %d", &id, &count)) == 2) {
			docCount_t *docCount = makeDocCount(id, count);
			qput(wdq -> qp, docCount);
		}
	}
	
	free(word);
	fclose(f);

	return index;
}
