#include <stdlib.h>
#include <errno.h>
#include "getline.h"

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
	if (lineptr == NULL || stream == NULL || n == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	size_t pos = 0;
	int c = fgetc(stream);
	if (c == EOF)
		return -1;

	if (*lineptr == NULL && *n == 0)
	{
		*lineptr = malloc(128);
		if (*lineptr == NULL)
			return -1;
		*n = 128;
	}

	while (c != EOF)
	{
		if (pos + 1 >= *n)
		{
			size_t new_size = *n + (*n >> 2);
			if (new_size < 128)
				new_size = 128;
			char *new_ptr = realloc(*lineptr, new_size);
			if (new_ptr == NULL)
				return -1;
			*n = new_size;
			*lineptr = new_ptr;
    		}

		((unsigned char *)(*lineptr))[pos++] = c;
		if (c == '\n')
			break;
		c = fgetc(stream);
	}

	(*lineptr)[pos] = '\0';
	return pos;
}
