#ifndef LINE_H
#define LINE_H

#include <string.h>

#define BUFF_SIZE 128

typedef struct line
{
	unsigned char *s;
	size_t len;
	size_t cap;
	struct line *prev;
	struct line *next;
} Line;

Line *line_insert(Line *prev, Line *next);
void line_delete(Line *line);
void line_insert_str(Line *line, size_t at, const unsigned char *str);
void line_delete_str(Line *line, size_t at, size_t len);
void line_insert_char(Line *line, size_t at, unsigned char c);
void line_delete_char(Line *line, size_t at);
void line_free(Line *line);

#endif
