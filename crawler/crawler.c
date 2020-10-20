#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <sys/stat.h>

int isDirectoryExists(const char *path)
{
	struct stat stats;
	stat(path, &stats);

	if (S_ISDIR(stats.st_mode))
		return 1;

	return 0;
}

void printInfo(void *vp)
{
	webpage_t *wp = (webpage_t *)vp;
	printf("Internal url: %s\n", webpage_getURL(wp));
}

void deleteWebPages(void *vp)
{
	webpage_t *wp = (webpage_t *)vp;
	webpage_delete(wp);
}

bool search(void *elementPtr, const void *keyPtr)
{
	char *url = (char *)elementPtr;
	char *key = (char *)keyPtr;

	return strcmp(key, url) == 0;
}

static const char crawlerfile[] = ".crawler";
bool pagedirinit(const char *dir)
{
	int filenamelength = strlen(dir) + strlen(crawlerfile) + 2;
	char *file = malloc(filenamelength);
	sprintf(file, "%s/%s", dir, crawlerfile);
	FILE *fp = fopen(file, "w");
	if (fp == NULL)
	{
		free(file);
		return false;
	}
	else
	{
		fclose(fp);
		free(file);
		return true;
	}
}

void parsearg(const int argc, char *argv[], char **seedurl, char **pagedir, int *maxdepth)
{
	if (argc != 4)
	{
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	*seedurl = argv[1];

	if (!NormalizeURL(*seedurl) || !IsInternalURL(*seedurl))
	{
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}

	*pagedir = argv[2];
	if (!pagedirinit(*pagedir))
	{
		printf("invalid directory\n");
		exit(EXIT_FAILURE);
	}

	char *maxdepthstring = argv[3];
	char excess;
	if (sscanf(maxdepthstring, "%d%c", maxdepth, &excess) != 1)
	{
		printf("invalid depth\n");
		exit(EXIT_FAILURE);
	}
}

int32_t pageSave(webpage_t *page, int id, char *dirName)
{
	char *location = (char *)malloc(50 * sizeof(char));

	if (page == NULL || dirName == NULL)
	{
		printf("Illegal arguments\n");
		return 1;
	}

	strcpy(location, dirName);

	if (isDirectoryExists(location) == 1)
	{
		strcat(location, "/");
		char idStr[5];
		sprintf(idStr, "%i", id);
		strcat(location, idStr);

		FILE *newFile = fopen(location, "w");
		fprintf(newFile, "%s\n%d\n%d\n%s", webpage_getURL(page), webpage_getDepth(page), webpage_getHTMLlen(page), webpage_getHTML(page));
		fclose(newFile);

		free(location);
		return 0;
	}
	else
	{
		printf(" Directory does not exist\n");
		free(location);
		return 1;
	}
}

void pageScan(webpage_t *page, queue_t *webQueue, hashtable_t *webHash)	
{
	if (page != NULL && webQueue != NULL && webHash != NULL){
	int pos = 0;
	char *result = NULL;

	while ((pos = webpage_getNextURL(page, pos, &result)) > 0)
	{

		if (IsInternalURL(result))
		{
			if (hsearch(webHash, search, result, strlen(result)) == NULL)
			{
				webpage_t *wp = webpage_new(result, webpage_getDepth(page)+1, NULL);
				qput(webQueue, wp);
				hput(webHash, result, result, strlen(result));
			}
			else
			{
				free(result);
			}
		}
		else
		{
			free(result);
		}
	}
	}
}

int32_t crawl(char *seedurl, char *pagedir, int maxdepth)
{
	queue_t *webQueue = qopen();
	if (webQueue == NULL)
	{
		return 1;
	}
	hashtable_t *webHash = hopen(70);
	if (webHash == NULL)
	{
		return 1;
	}

	char *seedcopy = malloc(strlen(seedurl) + 1);
	strcpy(seedcopy, seedurl);

	webpage_t *seedpage = webpage_new(seedcopy, 0, NULL);
	if (seedpage == NULL)
	{
		return 1;
	}

	hput(webHash, seedcopy, seedcopy, strlen(seedcopy));
	qput(webQueue, seedpage);

	int docid = 0;
	webpage_t *page;

	while ((page = qget(webQueue)) != NULL)
	{
		if (webpage_fetch(page))
		{
			pageSave(page, ++docid, pagedir);
			if (webpage_getDepth(page) < maxdepth)
			{
				pageScan(page, webQueue, webHash);
			}
		}
		webpage_delete(page);
	}

	qclose(webQueue);
	hclose(webHash);

	return 0;
}

int main(int argc, char *argv[])
{
	char *seedurl = NULL;
	char *pagedir = NULL;
	int maxdepth = 0;

	parsearg(argc, argv, &seedurl, &pagedir, &maxdepth);
	int32_t result = crawl(seedurl, pagedir, maxdepth);
	if (result != 0)
	{
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
