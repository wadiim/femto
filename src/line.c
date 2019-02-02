#include <stdlib.h>
#include "line.h"
#include "die.h"

Line *line_insert(Line *prev, Line *next)
{
	Line *line = (Line *)malloc(sizeof(Line));
	line->s = (unsigned char *)malloc(BUFF_SIZE);
	if (line == NULL || line->s == NULL)
		die("malloc");
	line->s[0] = '\0';
	line->len = 0;
	line->cap = BUFF_SIZE;
	line->prev = prev;
	line->next = next;
	if (prev)
		prev->next = line;
	if (next)
		next->prev = line;
	return line;
}

void line_delete(Line *line)
{
	if (line->prev && line->next)
	{
		line->prev->next = line->next;
		line->next->prev = line->prev;
	}
	else if (line->prev)
		line->prev->next = NULL;
	else if (line->next)
		line->next->prev = NULL;
	free(line->s);
	free(line);
}

void line_insert_str(Line *line, size_t at, const unsigned char *str)
{
	size_t len = line->len + strlen((char *)str) + 1;
	if (len >= line->cap)
	{
		while (len >= line->cap)
			line->cap += BUFF_SIZE;
		if (!(line->s = (unsigned char *)realloc(line->s, line->cap)))
			die("realloc");
	}
	if (at >= line->len)
		memmove(&line->s[at], str, strlen((char *)str) + 1);
	else
	{
		memmove(line->s + at + strlen((char *)str), line->s + at,
			(line->len - at) + 1);
		for (size_t i = at, j = 0; i < at +
			strlen((char *)str); ++i, ++j)
		{
			line->s[i] = str[j];
		}
	}
	line->s[len] = '\0';
	line->len += strlen((char *)str);
}

void line_delete_str(Line *line, size_t at, size_t len)
{
	if (line->len <= 0)
		return;
	if (at >= line->len)
		at = line->len - 1;
	if (at + len >= line->len)
		len -= at + len - line->len;
	memmove(line->s + at, line->s + at + len, (line->len - at));
	line->len -= len;
}

void line_insert_char(Line *line, size_t at, unsigned char c)
{
	line_insert_str(line, at, &c);
}

void line_delete_char(Line *line, size_t at)
{
	line_delete_str(line, at, 1);
}

void line_free(Line *line)
{
	while (line)
	{
		Line *temp = line;
		line = line->next;
		free(temp->s);
		free(temp);
	}
}
