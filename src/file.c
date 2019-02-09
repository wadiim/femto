#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "line.h"
#include "die.h"

void file_init(File *file)
{
	status_init();
	file->path = (char *)malloc(64);
	file->path[0] = '\0';
	file->cap = 64;
	file->cursor.x = 0;
	file->cursor.y = 0;
	buffer_init(&file->buffer);
	file->top = file->buffer.begin;
	file->is_modified = false;
}

void file_load(File *file)
{
	char *line = NULL;
	size_t len = 0;

	FILE *f = fopen(file->path, "r");
	if (!f)
		die("fopen");

	while(getline(&line, &len, f) != EOF && line)
	{
		for (size_t len = strlen(line); len > 0 &&
			(line[len - 1] == '\n' || line[len - 1] == '\r'); --len)
		{
			line[len - 1] = '\0';
		}
		line_insert_str(file->buffer.curr, 0, (unsigned char *)line);
		line_insert(file->buffer.curr, NULL);
		file->buffer.curr = file->buffer.curr->next;
	}
	file->buffer.curr = file->buffer.begin;
	free(line);
	fclose(f);
}

void file_save(File *file)
{
	FILE *f = fopen(file->path, "w");
	if (!f)
		die("fopen");

	for (Line *line = file->buffer.begin; line; line = line->next)
	{
		fputs((char *)line->s, f);
		fputs("\n", f);
	}
	fclose(f);
}

void file_free(File *file)
{
	buffer_free(&file->buffer);
	free(file->path);
	status_free();
}
