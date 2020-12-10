#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <sys/stat.h>


int isDirectoryExists(const char *path)
{
	struct stat stats;
	stat(path, &stats);

	if (S_ISDIR(stats.st_mode))
		return 1;

	return 0;
}


int32_t pagesave(webpage_t *page, int id, char *dirName)
{
	char *location = (char *)malloc(50 * sizeof(char));

	if (!page || !id || !dirName)
	{
		printf("Illegal arguments\n");
        free(location);
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


webpage_t *pageload(int id, char *dirnm)
{
	char *location = (char *)malloc(50 * sizeof(char));

	if (!id || !dirnm)
	{
		printf("Illegal arguments\n");
		free(location);
		return NULL;
	}

	strcpy(location, dirnm);
	if (isDirectoryExists(location) == 1)
	{
    strcat(location, "/");
		char idStr[5];
		sprintf(idStr, "%i", id);
		strcat(location, idStr);

		if (access(location, F_OK) != -1)
		{
			FILE *pageFile = fopen(location, "r");
			char url[100];
			int depth;
			int len;
			
			fscanf(pageFile, "%s\n%d\n%d\n", url, &depth, &len);
			
			char *html = malloc(sizeof(char) * len + 1);

			char c;
			int idx = 0;
			while ((c = fgetc(pageFile)) != EOF)
			{
				html[idx] = c;
				idx++;
			}
			html[idx] = '\0';
			
			fclose(pageFile);

			webpage_t *loadedPage = webpage_new(url, depth, html);
			
			free(location);
			return loadedPage;
		}
		else
		{
			printf("File doesn't exist");
			free(location);
			return NULL;
		}
  }
	else
	{
		printf(" Directory does not exist\n");
		free(location);
		return NULL;
	}
}
