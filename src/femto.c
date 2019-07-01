#include <locale.h>
#include <stdio.h>
#include <ctype.h>
#include "cursor.h"
#include "femto.h"
#include "utils.h"
#include "line.h"
#include "term.h"
#include "text.h"
#include "move.h"
#include "die.h"
#include "io.h"

void show_usage(char *name);

size_t fix_cursor_position(Window *window, Cursor *cursor);
void print(Window *window, Cursor *cursor, size_t rowsoff);

bool do_save(void);
void do_quit(unsigned char *s);

void init(int argc, char **argv)
{
	if (argc > 2)
		show_usage(argv[0]);

	term_init();
	setlocale(LC_ALL, "");
	file_init(&femto.file);
	femto.tabsize = get_tabsize();

	if (argc == 2)
	{
		size_t len = strlen(argv[1]) + 1;
		if (len >= femto.file.cap)
		{
			femto.file.path = (char *)realloc(femto.file.path, len);
			if (femto.file.path == NULL)
				die("realloc");
		}
		memmove(femto.file.path, argv[1], len);
		femto.file.path[len] = '\0';
		if (is_file_exist(femto.file.path))
			file_load(&femto.file);
		else
		{
			char *msg = "[New file]";
			size_t msglen = strlen(msg);
			size_t buflen = len + msglen + 2;
			char *buf = (char *)malloc(buflen + 1);

			memmove(buf, "\"", 2);
			memmove(&buf[1], femto.file.path, len - 1);
			memmove(&buf[len], "\" ", 2);
			memmove(&buf[len + 2], msg, msglen);

			buf[buflen] = '\0';
			status(buf);
			free(buf);
		}
	}
}

void refresh_screen(void)
{
	Window window;
	Cursor cursor = {
		length_to_width(femto.file.buffer.curr->s, femto.file.cursor.x),
		0
	};

	get_window_size(&window.x, &window.y);

	size_t rowsoff = fix_cursor_position(&window, &cursor);

	hide_cursor();
	set_cursor_position(0, 0);

	print(&window, &cursor, rowsoff);
	status_print(window.x);

	if (status_mode != INPUT)
		set_cursor_position(cursor.x, cursor.y);
	show_cursor();
}

void process_keypress(void)
{
	int special_key;
	unsigned char *s;
	size_t len = read_console(&s, &special_key);
	if (len != 0)
		insert(s);
	else
	{
		switch (special_key)
		{
		case BACKSPACE:
			do_backspace();
			break;
		case TAB:
			do_tab();
			break;
		case ENTER:
			do_enter();
			break;
		case HOME:
			do_home();
			break;
		case END:
			do_end();
			break;
		case PAGE_UP:
			do_page_up();
			break;
		case PAGE_DOWN:
			do_page_down();
			break;
		case ARROW_LEFT:
			do_left();
			break;
		case ARROW_UP:
			do_up();
			break;
		case ARROW_RIGHT:
			do_right();
			break;
		case ARROW_DOWN:
			do_down();
			break;
		case CTRL_HOME:
			do_top();
			break;
		case CTRL_END:
			do_bottom();
			break;
		case CTRL_ARROW_LEFT:
			do_prev_word();
			break;
		case CTRL_ARROW_RIGHT:
			do_next_word();
			break;
		case CTRL_KEY('s'):
			do_save();
			break;
		case CTRL_KEY('q'):
			do_quit(s);
			break;
		}
	}
	free(s);
}

size_t fix_cursor_position(Window *window, Cursor *cursor)
{
	size_t rowsoff = 0;

	if (cursor->x > line_width(femto.file.buffer.curr))
		cursor->x = line_width(femto.file.buffer.curr);

	for (Line *line = femto.file.top; line != femto.file.buffer.curr;
		line = line->next)
	{
		cursor->y += line_width(line) / window->x + 1;
		if (line->len != 0 && line_width(line) % window->x == 0)
			--cursor->y;
	}

	while (cursor->x >= window->x)
	{
		cursor->x -= window->x;
		++cursor->y;
	}

	while (cursor->y >= SCREEN_ROWS(window->y))
	{
		--cursor->y;
		++rowsoff;
	}

	while (rowsoff > 0 && femto.file.top->len == 0)
	{
		--rowsoff;
		femto.file.top = femto.file.top->next;
	}

	while (rowsoff >= LINE_ROWS(line_width(femto.file.top), window->x) &&
		line_width(femto.file.top) != 0 && femto.file.top->next)
	{
		rowsoff -= LINE_ROWS(line_width(femto.file.top), window->x);
		femto.file.top = femto.file.top->next;
	}

	return rowsoff;
}

void print(Window *window, Cursor *cursor, size_t rowsoff)
{
	size_t width, offset, row, len;
	Line *top = femto.file.top;

	for (row = 0; (row < SCREEN_ROWS(window->y)) && top;
		++row, top = top->next)
	{
		len = width_to_length(top->s,
			(rowsoff > 0 ? rowsoff : 1) * window->x);
		offset = (row == 0 && rowsoff > 0) ? len : 0;
		width = str_width(top->s + offset, top->len - offset);

		if (width >= window->x)
		{
			for (; width >= window->x &&
				row++ < SCREEN_ROWS(window->y);
				width -= window->x)
			{
				len = width_to_length(top->s + offset,
					window->x);
				clear_line();
				writeln_console(top->s + offset, len);
				offset += len;
			}
			if (width > 0 && row < SCREEN_ROWS(window->y))
			{
				clear_line();
				writeln_console(top->s + offset,
					top->len - offset);
			}
			else
				--row;
		}
		else
		{
			clear_line();
			writeln_console(top->s + offset, top->len - offset);
		}
	}

	if (cursor->y > row - 1 && row > 0 &&
		(line_width(femto.file.buffer.curr)
		% window->x != 0 || femto.file.buffer.curr == 0))
	{
		cursor->y = row - 1;
	}

	for (; row < SCREEN_ROWS(window->y); ++row)
	{
		clear_line();
		writeln_console((unsigned char *)"", 0);
	}
}

bool do_save(void)
{
	bool retval = false;
	Line *input = line_insert(NULL, NULL);
	if (status_input(input, "Save as: ", femto.file.path))
	{
		if (femto.file.path != (char *)input->s)
		{
			if (input->len >= femto.file.cap)
			{
				femto.file.path =
					(char *)realloc(femto.file.path,
						input->len + 1);
				if (femto.file.path == NULL)
					die("realloc");
			}
			memmove(femto.file.path, input->s, input->len + 1);
			femto.file.path[input->len] = '\0';
		}
		file_save(&femto.file);
		femto.file.is_modified = false;
		retval = true;
	}
	line_free(input);
	return retval;
}

void do_quit(unsigned char *s)
{
	bool quit = true;
	if (femto.file.is_modified)
	{
		Line *input = line_insert(NULL, NULL);
		if (status_input(input, "Do you want to save changes?: ", NULL))
		{
			if (tolower(input->s[0]) != 'n' && !do_save())
				quit = false;
		}
		else
			quit = false;

		line_free(input);
	}
	if (quit)
	{
		file_free(&femto.file);
		free(s);
		exit(0);
	}
}

void show_usage(char *name)
{
	printf("Usage: %s [file]\n", name);
	exit(0);
}
