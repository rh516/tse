/* indexio.h
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <hash.h>

/* indexsave() - saves index (hashtable of the document queues)
 * saves index to file in format
 * <word> <docID1> <count1> <docID2> <count2> ...
 * return 0 on sucess and -1 for failure
 */

int32_t indexsave(hashtable_t *index, char *fname);

hashtable_t *indexload(char *fname);
