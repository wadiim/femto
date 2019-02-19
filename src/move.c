#include <stdbool.h>
#include <string.h>
#include "femto.h"
#include "utils.h"
#include "move.h"
#include "term.h"
#include "file.h"
#include "line.h"

void move_up(void);
void move_down(void);

void do_home(void)
{
	size_t pos = find_first_nonblank(femto.file.buffer.curr->s);
	if (pos < femto.file.buffer.curr->len && femto.file.cursor.x != pos)
		femto.file.cursor.x = pos;
	else
		femto.file.cursor.x = 0;
}

void do_end(void)
{
	femto.file.cursor.x = line_mblen(femto.file.buffer.curr);
}

void do_page_up(void)
{
	size_t y, i;
	get_window_size(NULL, &y);
	for (i = y - 1; i > 0 && femto.file.top->prev &&
		femto.file.buffer.curr->prev; --i)
	{
		move_up();
		femto.file.top = femto.file.top->prev;
	}
	if (femto.file.cursor.y != 0 && femto.file.top->prev == NULL)
	{
		for (; i > 0 && femto.file.buffer.curr->prev; --i)
			move_up();
	}
	fix_cursor_x();
}

void do_page_down(void)
{
	size_t y;
	get_window_size(NULL, &y);
	for (size_t i = 0; i < y - 1 && femto.file.top->next &&
		femto.file.buffer.curr->next; ++i)
	{
		move_down();
		femto.file.top = femto.file.top->next;
	}
	fix_cursor_x();
}

void do_left(void)
{
	if (femto.file.cursor.x > 0)
		--femto.file.cursor.x;
}

void do_up(void)
{
	if (femto.file.buffer.curr->prev)
	{
		if (femto.file.buffer.curr == femto.file.top)
			femto.file.top = femto.file.top->prev;
		move_up();
		fix_cursor_x();
	}
}

void do_right(void)
{
	if (femto.file.cursor.x < line_mblen(femto.file.buffer.curr))
		++femto.file.cursor.x;
}

void do_down(void)
{
	if (femto.file.buffer.curr->next)
	{
		move_down();
		fix_cursor_x();
	}
}

void move_up(void)
{
	--femto.file.cursor.y;
	femto.file.buffer.curr = femto.file.buffer.curr->prev;
}

void move_down(void)
{
	++femto.file.cursor.y;
	femto.file.buffer.curr = femto.file.buffer.curr->next;
}

void do_prev_word(void)
{
	unsigned char *str = femto.file.buffer.curr->s;
	size_t pos = mbnum_to_index(str, femto.file.cursor.x);
	bool seen_a_word = false, step_forward = false;

	for (;;)
	{
		if (pos == 0)
		{
			if (femto.file.buffer.curr->prev == NULL)
				break;
			femto.file.buffer.curr = femto.file.buffer.curr->prev;
			pos = femto.file.buffer.curr->len;
			--femto.file.cursor.y;
		}
		pos = move_mbleft(femto.file.buffer.curr->s, pos);
		if (is_alnum_mbchar(femto.file.buffer.curr->s + pos))
		{
			seen_a_word = true;
			if (pos == 0)
				break;
		}
		else if (seen_a_word)
		{
			step_forward = true;
			break;
		}
	}

	if (step_forward)
		pos = move_mbright(femto.file.buffer.curr->s, pos);

	femto.file.cursor.x = index_to_mbnum(str, pos);
}

void do_next_word(void)
{
	unsigned char *str = femto.file.buffer.curr->s;
	size_t pos = mbnum_to_index(str, femto.file.cursor.x);
	bool started_on_word = is_alnum_mbchar(femto.file.buffer.curr->s + pos);
	bool seen_space = !started_on_word;

	for (;;)
	{
		if (femto.file.buffer.curr->s[pos] == '\0')
		{
			if (femto.file.buffer.curr->next == NULL)
				break;
			femto.file.buffer.curr = femto.file.buffer.curr->next;
			++femto.file.cursor.y;
			seen_space = true;
			pos = 0;
		}
		else
			pos = move_mbright(femto.file.buffer.curr->s, pos);

		if (!is_alnum_mbchar(femto.file.buffer.curr->s + pos))
			seen_space = true;
		else if (seen_space)
			break;
	}

	femto.file.cursor.x = index_to_mbnum(str, pos);
}

void do_top(void)
{
	femto.file.cursor.x = femto.file.cursor.y = 0;
	femto.file.top = femto.file.buffer.curr = femto.file.buffer.begin;
}

void do_bottom(void)
{
	while (femto.file.buffer.curr->next)
	{
		++femto.file.cursor.y;
		femto.file.buffer.curr = femto.file.buffer.curr->next;
	}

	do_end();
	femto.file.top = femto.file.buffer.curr;

	size_t y;
	get_window_size(NULL, &y);
	while (--y && femto.file.top->prev)
		femto.file.top = femto.file.top->prev;
}
