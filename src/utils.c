#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "status.h"
#include "femto.h"
#include "utils.h"
#include "term.h"
#include "line.h"
#include "die.h"
#include "io.h"

void fix_cursor_x(void)
{
	size_t len = line_mblen(femto.file.buffer.curr);
	if (femto.file.cursor.x > len)
		femto.file.cursor.x = len;
}

size_t get_tabsize(void)
{
	size_t x;
	set_cursor_position(0, 0);
	write_console((unsigned char *)"\t", 1);
	get_cursor_position(&x, NULL);
	return x;
}

char *size_t_to_str(size_t num)
{
	size_t len = snprintf(NULL, 0, "%zu", num);
	char *str = (char *)malloc(len + 1);
	if (str == NULL)
		die("malloc");
	snprintf(str, len + 1, "%zu", num);
	str[len] = '\0';
	return str;
}

size_t str_width(const unsigned char *s, size_t len)
{
	return length_to_width(s, len);
}

void fill_with_spaces(char *s, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		s[i] = ' ';
}

size_t length_to_width(const unsigned char *s, size_t len)
{
	size_t col = 0;
	for (size_t i = 0; i < len; ++i)
	{
		if (s[i] == '\t')
			col += femto.tabsize - col % femto.tabsize;
		else if (!is_continuation_byte(s[i]))
			++col;
		else if (s[len] != 0)
			++len;
	}
	return col;
}

size_t width_to_length(const unsigned char *s, size_t width)
{
	size_t len = 0, tabsize;
	for (size_t col = 0; width > 0; ++len)
	{
		if (s[len] == '\t')
		{
			tabsize = femto.tabsize - col % femto.tabsize;
			width -= tabsize;
			col += tabsize;
		}
		else if (!is_continuation_byte(s[len]))
		{
			++col;
			--width;
		}
	}
	while (s[len] != 0 && is_continuation_byte(s[len]))
		++len;
	return len;
}

size_t find_first_nonblank(const unsigned char *s)
{
	size_t i = 0;
	while (isspace(s[i]) && s[i++] != 0)
		;
	return i;
}
