#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <webpage.h>
#include <pageio.h>

int main(void) {
    char *seedUrl;
    char *pageDir;

    seedUrl = "https://thayer.github.io/engs50/";
    pageDir = "pages";

    // webpage_t *testPage = webpage_new(seedUrl, 0, NULL);

    // pagesave(testPage, 1, pageDir);
    webpage_t *testPage = pageload(1, pageDir);
    pagesave(testPage, 2, "pages");
    return 0;
}
