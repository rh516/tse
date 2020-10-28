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

// hashtable_t *indexload(char *fname) {
// 	FILE *f = fopen(fname, "r");
// 	if (f == NULL) {
// 		printf("indexload failed, cannot open file\n");
// 		return -1;
// 	}

// 	hashtable_t *index = hopen(500);
// 	char *line;
// 	char *s = " ";
	

// 	//go through file line by line
// 	while (fgets(line, sizeof(line), f) != NULL) {
// 		char *word;
// 		char *token;

// 		// split line at every space
// 		token = strtok(line, s);

// 		// word is first token
// 		word = token;

// 		wordDocQueue_t *docsq = malloc(sizeof(wordDocQueue_t));
// 		docsq->qp = qopen();

// 		hput(index, docsq, word, sizeof(word));

// 		// now parse through rest of tokens which include document IDs and counts
// 		char *id;
// 		char *count;

// 		while (token != NULL) {
// 			token = strtok(NULL, s);
// 			id = token;
// 			token = strtok(NULL, s);

// 			if (token != NULL) {
// 				count = token;
// 				docCount_t *doc = malloc(sizeof(docCount_t));
// 				doc->id = atoi(id);
// 				doc->count = atoi(count);
// 				qput(docsq->qp, doc);
// 			}
// 		}
// 	}
// 	fclose(f);
// 	return index;
// }
			