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

bool isValid(char *c)
{
  char *curr = c;
  while ((int)(*curr) != 10)
  {
    if (!isalpha(*curr))
    {
      if (!((int)(*curr) == 9 || (int)(*curr) == 32))
      {
        return false;
      }
    }
    curr += 1;
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
void replace(queue_t *all, queue_t *check)
{
  queue_t *backup = qopen();
  docCount_t *curr;

  while ((curr = qget(all)) != NULL)
  {
    docCount_t *pointer = qsearch(check, docSearch, curr->id);
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

int rank(char **wordArr, hashtable_t **index, queue_t *all, int max)
{
  if (wordArr == NULL)
  {
    return 3;
  }

  wordDocQueue_t *searchresult = hsearch(index, wordSearch, wordArr[0], strlen(wordArr[0]));
  if (searchresult == NULL)
  {
    return 3;
  }

  queue_t *backup = qopen();
  docCount_t *curr;

  while ((curr = qget(searchresult->qp)) != NULL)
  {
    docCount_t *doc = makeDocCount(curr->id, curr->count);
    qput(all, doc);
    qput(backup, curr);
  }
  qconcat(searchresult->qp, backup);

  for (int i = 1; i < max; i++)
  {
    char *word = wordArr[i]; // my current word <3
    if (strcmp(word, "and") != 0 && strlen(word) > 2)
    {
      wordDocQueue_t *searchresult = hsearch(index, wordSearch, word, strlen(word));
      if (searchresult == NULL)
      {
        return 3;
      }
      replace(all, searchresult->qp);
    }
  }
}

int main(void)
{
  char input[101];
  hashtable_t *index = indexload("../indexer/index1");

  printf("> ");
  while (fgets(input, 101, stdin) != NULL)
  {
    if (!isValid(input))
    {
      printf("Invalid query \n");
    }
    else
    {
      NormalizeWord(input);
      input[strlen(input) - 1] = '\0';

      char *token = strtok(input, " ");
      char *wordArr[50];
      int i = 0;

      while (token != NULL)
      {
        wordArr[i++] = token;
        token = strtok(NULL, " ");
      }
      queue_t *all = qopen();
      if (rank(wordArr, index, all, i) != 0)
      {
        // free wordarr, close all, print statement
        free(wordArr);
        qclose(all);
        printf("word was not in index\n");
        continue; //goes onto the next line of input
      }
      printf("> ");
    }

    happly(index, freeQ);
    hclose(index);
    printf("\n");
    return 0;
  }