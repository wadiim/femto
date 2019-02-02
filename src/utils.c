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
	size_t len = num_of_mbchars(femto.file.buffer.curr);
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

size_t utf8_len(unsigned char c)
{
	if ((c & (1 << 7)) == 0)
		return 1;
	if ((c & (1 << 6)) == 0)
		return UTF8_CONTINUATION_BYTE;
	if ((c & (1 << 5)) == 0)
		return 2;
	if ((c & (1 << 4)) == 0)
		return 3;
	return 4;
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

size_t line_width(Line *line)
{
	return get_column(line->s, line->len);
}

size_t str_width(const unsigned char *s, size_t len)
{
	return get_column(s, len);
}

void fill_with_spaces(char *s, size_t len)
{
	for (size_t i = 0; i < len; ++i)
		s[i] = ' ';
}

size_t get_column(const unsigned char *s, size_t at)
{
	size_t col = 0;
	for (size_t i = 0; i < at; ++i)
	{
		if (s[i] == '\t')
			col += femto.tabsize - col % femto.tabsize;
		else if (!is_continuation_byte(s[i]))
			++col;
		else if (s[at] != 0)
			++at;
	}
	return col;
}

size_t num_of_mbchars(Line *line)
{
	size_t num = 0;
	for (size_t i = 0; i < line->len; ++i)
	{
		if (!is_continuation_byte(line->s[i]))
			++num;
	}
	return num;
}

size_t mbchar_position(const unsigned char *s, size_t n)
{
	size_t pos = 0;
	for (size_t i = 0; i < n; ++i)
	{
		if (!is_continuation_byte(s[pos]))
			++pos;
		while (is_continuation_byte(s[pos]))
			++pos;
	}
	return pos;
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
