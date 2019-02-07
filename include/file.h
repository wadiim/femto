#ifndef FILE_H
#define FILE_H

#ifdef __STDC_ALLOC_LIB__
# define __STDC_WANT_LIB_EXT2__ 1
#else
# define _POSIX_C_SOURCE 200809L
#endif

#ifndef _DEFAULT_SOURCE
# define _DEFAULT_SOURCE
#endif

#ifndef _BSD_SOURCE
# define _BSD_SOURCE
#endif

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#ifdef _WIN32
# include "getline.h"
#endif

#include <stdbool.h>
#include "buffer.h"
#include "cursor.h"
#include "status.h"

typedef struct line Line;

typedef struct
{
	size_t cap;
	char *path;
	Line *top;
	Buffer buffer;
	Cursor cursor;
	Status status;
	bool is_modified;
} File;

void file_init(File *file);
void file_load(File *file);
void file_save(File *file);
void file_free(File *file);

#endif
