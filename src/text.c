#include <stdlib.h>
#include "femto.h"
#include "utils.h"
#include "text.h"
#include "line.h"
#include "file.h"
#include "io.h"

void delete_character(void);
void delete_empty_line(void);
void break_line_into_two(void);
void concatenate_with_previous_line(void);

void insert(const unsigned char *s)
{
	line_insert_str(femto.file.buffer.curr,
		mbchar_position(femto.file.buffer.curr->s,
			femto.file.cursor.x), s);
	femto.file.is_modified = true;
	++femto.file.cursor.x;
}

void do_backspace(void)
{
	if (femto.file.cursor.x > 0 && femto.file.buffer.curr->len > 0)
	{
		delete_character();
	}
	else if (femto.file.cursor.x == 0 &&
		femto.file.buffer.curr->len == 0 &&
		femto.file.buffer.curr->prev != NULL)
	{
		delete_empty_line();
	}
	else if (femto.file.cursor.x == 0 &&
		femto.file.buffer.curr->len > 0 &&
		femto.file.buffer.curr->prev != NULL)
	{
		concatenate_with_previous_line();
	}
	femto.file.is_modified = true;
}

void do_enter(void)
{
	line_insert(femto.file.buffer.curr, femto.file.buffer.curr->next);

	if (femto.file.cursor.x < num_of_mbchars(femto.file.buffer.curr))
		break_line_into_two();

	femto.file.buffer.curr = femto.file.buffer.curr->next;
	femto.file.cursor.x = 0;
	++femto.file.cursor.y;
	femto.file.is_modified = true;
}

void do_tab(void)
{
	insert((unsigned char *)"\t");
}

void delete_character(void)
{
	size_t char_len = 1;
	size_t i = mbchar_position(femto.file.buffer.curr->s,
		femto.file.cursor.x);

	while (is_continuation_byte(femto.file.buffer.curr->s[--i]))
		++char_len;

	line_delete_str(femto.file.buffer.curr, i, char_len);

	--femto.file.cursor.x;
	fix_cursor_x();
}

void delete_empty_line(void)
{
	if (femto.file.buffer.curr == femto.file.top && femto.file.top->prev)
		femto.file.top = femto.file.top->prev;
	femto.file.buffer.curr = femto.file.buffer.curr->prev;
	femto.file.cursor.x = num_of_mbchars(femto.file.buffer.curr);
	--femto.file.cursor.y;
	line_delete(femto.file.buffer.curr->next);
}

void break_line_into_two(void)
{
	size_t at = mbchar_position(femto.file.buffer.curr->s,
		femto.file.cursor.x);
	unsigned char *line = (unsigned char *)malloc(
		femto.file.buffer.curr->len - at + 1);
	memmove(line, femto.file.buffer.curr->s + at,
		femto.file.buffer.curr->len - at + 1);
	line_insert_str(femto.file.buffer.curr->next, 0, line);
	line_delete_str(femto.file.buffer.curr, at,
		femto.file.buffer.curr->len - at);
	free(line);
}

void concatenate_with_previous_line(void)
{
	unsigned char *line = (unsigned char *)malloc(
		femto.file.buffer.curr->len + 1);
	memmove(line, femto.file.buffer.curr->s,
		femto.file.buffer.curr->len);
	line[femto.file.buffer.curr->len] = '\0';
	femto.file.cursor.x = num_of_mbchars(femto.file.buffer.curr->prev);
	line_insert_str(femto.file.buffer.curr->prev,
		femto.file.buffer.curr->prev->len, line);
	femto.file.buffer.curr = femto.file.buffer.curr->prev;
	line_delete(femto.file.buffer.curr->next);
	--femto.file.cursor.y;
	free(line);
}
