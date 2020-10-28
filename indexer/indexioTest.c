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

int main(void) {
  hashtable_t *loaded = indexload("output1");
  indexsave(loaded, "output2");

  happly(loaded, freeQ);
  hclose(loaded);

  FILE *fp1 = fopen("output1", "r");
  FILE *fp2 = fopen("output2", "r");

  int ch1 = getc(fp1);
  int ch2 = getc(fp2);

  while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
    ch1 = getc(fp1);
    ch2 = getc(fp2);
  }

  if (ch1 == ch2)
    printf("Files are identical \n");
  else if (ch1 != ch2)
    printf("Files are Not identical \nn");

  fclose(fp1);
  fclose(fp2);


  return 0;
}