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

FILE *out;

bool wordSearch(void *word, const void *wordKey)
{
  if (word != NULL && wordKey != NULL)
  {
    char *w = (char *)wordKey;
    wordDocQueue_t *wdq = (wordDocQueue_t *)word;

    if (strcmp(w, wdq->word) == 0)
    {
      return true;
    }
  }
  return false;
}
// to see if a document is matching
bool sortsearch(void *element, const void *key)
{
  if (element == NULL)
  {
    return false;
  }
  docCount_t *doc = (docCount_t *)element; // cast !
  return doc->count == *(int *)key;
}

bool dir_exists(char *dirname) {                                                
  static const char crawlerfile[] = ".crawler";                                 
  int filenamelength = strlen(dirname) + strlen(crawlerfile) + 2;               
  char *file = malloc(filenamelength);                                          
                                                                                
  sprintf(file, "%s/%s", dirname, crawlerfile);                                 
                                                                                
  FILE *fp = fopen(file, "w");                                                  
  if (fp == NULL) {                                                             
    free(file);                                                                 
    return false;                                                              \
                                                                                
  }                                                                             
  else {                                                                        
    fclose(fp);                                                                 
    free(file);                                                                 
    return true;                                                                
  }                                                                             
}

bool isValid(char *line) {
  for (int i=0; i<strlen(line); i++) {
		if (isalpha(line[i]) == 0 && isspace(line[i]) == 0) {
			return false;
		}
		if (line[i] == '\t') {
			line[i] = ' ';
		}
	}
  return true;
}

void NormalizeWord(char *word)
{
  for (char *alpha = word; *alpha != '\0'; alpha++)
  {
    *alpha = tolower(*alpha);
  }
}

// cmp takes in 2 pointers, convert b and a to int pointers, dereference, then b-a
// basically = given 2 numbers, return to me the difference of the 2 numbers
int cmp(const void *a, const void *b)
{
  return *(int *)b - *(int *)a;
}

bool docSearch(void *doc, const void *id)
{
  int *docID = (int *)id;
  docCount_t *dp = (docCount_t *)doc;

  if (docID != NULL && doc != NULL)
  {
    if (dp->id == *docID)
    {
      return true;
    }
  }
  return false;
}

void freeQ(void *ep)
{
  wordDocQueue_t *temp = (wordDocQueue_t *)ep;
  free(temp->word);
  qclose(temp->qp);
}

// go thru the all queue, check if everything in the "all" queue is in the "check" queue,
// if not, remove! else, update (if lower value)
void update(queue_t *all, queue_t *check)
{
  queue_t *backup = qopen();
  docCount_t *curr;

  while ((curr = qget(all)) != NULL)
  {
    docCount_t *pointer = qsearch(check, docSearch, &curr->id);
    if (pointer == NULL)
    {
      free(curr); //if it's not in there, you want to remove it.
    }
    // if count of the pointer < the count of the curr, should update it!
    // why compare: bc i want the minimum boi
    else
    {
      if (pointer->count < curr->count)
      {
        curr->count = pointer->count;
      }
      qput(backup, curr); //regardless of comparison, should put curr into backup
    }
  }
  qconcat(all, backup); //backup is going INTO all
}

void copy(queue_t *all, queue_t *searchres) {
	if (searchres == NULL) {
		return;
	}
	queue_t *backup = qopen();
	docCount_t *curr;

	while ((curr= qget(searchres)) != NULL) {
		docCount_t *doc = malloc(sizeof(docCount_t));
		doc->id = curr->id;
		doc->count = curr->count;
		qput(backup, curr);
		qput(all, doc);
	}

	qconcat(searchres, backup);
}
			
	// combine
void combine(queue_t *all, queue_t *temp) {
	docCount_t *curr;
	while ((curr = qget(temp)) != NULL) {
		docCount_t * tempdoc;
		if((tempdoc = qsearch(all, docSearch, &(curr->id))) != NULL) {
			tempdoc->count += curr->count;
			free(curr);
		}
		else {
			qput(all, curr);
		}
	}
	qclose(temp);
}
						 
int rank(char **wordArr, hashtable_t *index, queue_t *all, int max)
{
  if (wordArr == NULL)
  {
    return 3;
  }

  queue_t *temp = NULL;
	
  for (int i = 0; i < max; i++) {
    char *word = wordArr[i]; // my current word <3
		if (strcmp(word,"or") == 0) {
			combine(all, temp);
			temp = NULL;
		}
    else if (strcmp(word, "and") != 0 && strlen(word) > 2){
      wordDocQueue_t *searchresult = hsearch(index, wordSearch, word, strlen(word));
			if (searchresult == NULL) {
				qclose(temp);
				temp = NULL;
				int j = i + 1;
				while (j < max) {
					char *word2 = wordArr[j];
					if (strcmp(word2, "or") == 0) {
						break;
					}
					j++;
				}
				i = j-1;
      }

			else if (temp == NULL) {
				temp = qopen();
				copy(temp, searchresult->qp);
			}
			
			else {
				update(temp, searchresult->qp);
			}
    }
	}
	if (temp != NULL) {
		combine(all, temp);
		temp = NULL;
	}
  return 0;
}

// it only takes in the queue "all"
int sort(queue_t *all)
{
  //must make a backup queue - so after sorted, then put into the original queue
  queue_t *backup = qopen();
  docCount_t *curr;
  int i = 0;
  int holder[500]; //int array
  int ret = 0;

  while ((curr = qget(all)) != NULL) // comparing value of curr against null
  {
    qput(backup, curr);
    holder[i] = curr->count; //sort "holder" array (where each value is a count of a specific doc) and use it
    i++;
  }

  // sort the queue if it's not empty dawg
  if (i > 0)
  {
    qsort(holder, i, sizeof(int), cmp);
    for (int k = 0; k < i; k++)
    {
      // store the count of the first ting
      int rank = holder[k];
      docCount_t *data = qremove(backup, sortsearch, &rank);
      qput(all, data);
    }
  }
  else
  {
    ret = -1;
  }
  qclose(backup);
  return ret;
}

void printcount(void *element)
{
  if (element != NULL)
  {
    docCount_t *doc = (docCount_t *)element;
    char *dir = "../crawler/pages";
    char filename[100];
    sprintf(filename, "%s/%d", dir, doc->id);
    char url[100];
    FILE *fp = fopen(filename, "r"); //read mode
    // let's open up this file, scan, and put
    fscanf(fp, "%s", url);
    fclose(fp);
    fprintf(out,"rank: %d: doc: %d : %s\n", doc->count, doc->id, url);
  }
}

int parse(char *line, char **words)
{
  const char s[2] = " ";
  char *token;
  token = strtok(line, s); //split input by space
  // if the first word is "and" or "or", that's not a valid query
  if (strcmp(token, "and") == 0 || strcmp(token, "or") == 0) {
    return -1;
  }
  NormalizeWord(token);
  words[0] = token;
  token = strtok(NULL, s);

  int i = 1;
	while (token != NULL){
    NormalizeWord(token);
		words[i] = token;
		if (strcmp(words[i-1], "and") == 0 || strcmp(words[i-1], "or")== 0) {
				if (strcmp(words[i], "or") == 0 || strcmp(words[i], "and") == 0) {
				printf("cannot have adjacent ands and ors");
				}
		}
    token = strtok(NULL, s);
    i++;
  }

  if (strcmp(words[i - 1], "and") == 0 || strcmp(words[i - 1], "or") == 0)
  {
    return -1;
  }
  return i;
}

bool quietflag(const int argc, char *argv[]) {
	if (argc == 6) {
		if (strcmp(argv[3], "-q") == 0) {
			return true;
		}
	}
	return false;
}

int main(const int argc, char *argv[]) {

	// set in and out to stdin and stfout
	FILE *in = stdin;
	out = stdout;

	
	// check if valid number of arguments passed in
	if (argc > 3 && (strcmp(argv[3],"-q") != 0)){
		printf("usage: query <pageDirectory> <indexFile> [-q]\n");
		exit(1);
	}

	if (!dir_exists(argv[1])) {
		printf("%s is not a valid directory\n", argv[1]);
		exit(2);
	}

	FILE *indexfile;
	if((indexfile = fopen(argv[2], "r+")) == NULL) {
		printf("cannot read indexfile");
		exit(3);
	}
	fclose(indexfile);
	
  char input[500];

	//open input file is flag present
	if (quietflag(argc, argv)) {
		in = fopen(argv[4], "r");
		out = fopen(argv[5], "w");
		if (in == NULL) {
			printf("cannot read input file\n");
			fclose(out);
			exit(3);
		}
}
	else {
		printf("> ");
	}

hashtable_t *index = indexload(argv[2]);
	
  while (fgets(input, 500, in) != NULL) {
    if (!isValid(input)) {
      fprintf(out, "Invalid input \n> ");
      continue;
    }
    if (strlen(input) <= 1) {
      fprintf(out, "> ");
      continue;
    }
    input[strlen(input) - 1] = '\0';

    //parse the query and make a wordArr
    int maxwords = strlen(input) / 2;
    // malloc for arrays
    char **wordArr = calloc(maxwords, sizeof(char *));
    fprintf(out, "input: %s\n", input);

    // limit tells us how many words there are actually
    int limit = parse(input, wordArr);

    if (limit < 0) {
      fprintf(out, "invalid query\n> ");
      free(wordArr);
			continue;
		}

    queue_t *all = qopen();
    if (rank(wordArr, index, all, limit) != 0) {
      // free wordarr, close all, print statement
      free(wordArr);

      fprintf(out, "error occured \n> ");
      qclose(all);
      continue; //goes onto the next line of input
    }

    // we need to put it in order! sort em!a
    // we will pass "all" to the "sort" function
    int ret = sort(all);

    if (ret < 0) {
      fprintf(out, "no result\n ");
    }
    qapply(all, printcount);

    //reset
    free(wordArr);
    qclose(all);
    fprintf(out, "> ");
  }

	// check if -q present and then close files
	if (quietflag(argc, argv)) {
		fclose(in);
		fclose(out);
	}

  happly(index, freeQ);
hclose(index);
  return 0;
}
