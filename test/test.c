#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <webpage.h>
#include <pageio.h>

int main(void) {
    char *pageDir;

    pageDir = "pages";

    webpage_t *testPage1 = pageload(1, pageDir);
    pagesave(testPage1, 2, "pages");
    webpage_t *testPage2 = pageload(2, pageDir);

    printf("%d", strcmp(webpage_getURL(testPage1), webpage_getURL(testPage2)));
    if (webpage_getDepth(testPage1) == webpage_getDepth(testPage2)) 
    {
        printf("equal depth\n");
    }
    else
    {
        printf("not equal depth\n");
    }
    if (webpage_getHTMLlen(testPage1) == webpage_getHTMLlen(testPage2)) 
    {
        printf("equal html len\n");
    }
    else
    {
        printf("not html len\n");
    }
    printf("%d", strcmp(webpage_getHTML(testPage1), webpage_getHTML(testPage2)));
    


    webpage_delete(testPage1);
    webpage_delete(testPage2);
    return 0;
}
