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


void savedoc(void *d) {
	docCount_t *tempdoc = (docCount_t *)d;
	fprintf(f, "%d %d ", tempdoc->id, tempdoc->count);
}

void savewords(void *q) {
	wordDocQueue_t *tempQ = (wordDocQueue_t *)q;
	fprintf(f, "%s ", tempQ->word);
	qapply(tempQ->qp, savedoc);
}


int32_t indexsave(hashtable_t *index, char *fname) {
	FILE *f = fopen(fname, "w");
	if (f == NULL) {
		printf("failed to open file");
		return 1;
	}

	happly(index, savewords);
	fclose(f);
	fprintf(f, "\n");
	
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
			
		

