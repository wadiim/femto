#include "status.h"
#include "cursor.h"
#include "utils.h"
#include "femto.h"
#include "file.h"
#include "term.h"
#include "line.h"
#include "die.h"
#include "io.h"

Status_input *statin;

void status_realloc(size_t len);
void status_set_default(size_t len);

void status_do_insert(unsigned char *s);

void status_do_end(void);
void status_do_home(void);
void status_do_backspace(void);
void status_do_arrow_left(void);
void status_do_arrow_right(void);

void status_input_print(void);
bool status_process_input(void);

void status_input_init(void);
void status_input_free(void);

void status_init(void)
{
	if (!(femto.file.status.msg = (char *)malloc(BUFF_SIZE)))
		die("malloc");
	femto.file.status.msg[0] = '\0';
	femto.file.status.cap = BUFF_SIZE;
	femto.file.status.len = 0;
	status_mode = NORMAL;
}

void status_free(void)
{
	free(femto.file.status.msg);
	femto.file.status.cap = 0;
	femto.file.status.len = 0;
}

void status(const char *msg)
{
	size_t len = strlen(msg) + 1;
	status_realloc(len);
	memmove(femto.file.status.msg, msg, len);
	femto.file.status.msg[len] = '\0';
	femto.file.status.len = len - 1;
	status_mode = MESSAGE;
}

void status_print(size_t cols)
{
	switch (status_mode)
	{
	case NORMAL:
		status_set_default(cols);
	case MESSAGE:
		clear_line();
		write_console((unsigned char *)femto.file.status.msg, cols - 1);
		status_mode = NORMAL;
		break;
	case INPUT:
		status_input_print();
		break;
	}
}

bool status_input(Line *input, char *msg, const char *placeholder)
{
	status_input_init();

	statin->msg = msg;
	statin->input = input;

	if (placeholder)
	{
		line_insert_str(statin->input, 0, (unsigned char *)placeholder);
		statin->cx += strlen(placeholder);
	}

	return status_process_input();
}

bool status_process_input(void)
{
	int special_key;
	unsigned char *s;
	status_mode = INPUT;

	for (;;)
	{
		status_input_print();
		size_t len = read_console(&s, &special_key);
		if (len != 0)
			status_do_insert(s);
		else
		{
			switch (special_key)
			{
			case ESC:
			case ENTER:
				free(s);
				status_input_free();
				status_mode = NORMAL;
				return (special_key == ESC) ? false : true;
			case HOME:
				status_do_home();
				break;
			case END:
				status_do_end();
				break;
			case ARROW_LEFT:
				status_do_arrow_left();
				break;
			case ARROW_RIGHT:
				status_do_arrow_right();
				break;
			case BACKSPACE:
				status_do_backspace();
				break;
			}
		}
		free(s);
	}
}

void status_input_print(void)
{
	Window window;
	get_window_size(&window.x, &window.y);

	char *msg = statin->msg;
	size_t msglen = strlen(statin->msg);
	if (msglen >= window.x - 2)
	{
		msg = ": ";
		msglen = 2;
	}

	size_t free_space = window.x - msglen - 1;

	if (statin->cx < statin->charsoff)
		statin->charsoff = statin->cx;
	else if (statin->cx - statin->charsoff > free_space)
		statin->charsoff = statin->cx - free_space;

	size_t len;
	size_t input_width = line_width(statin->input);
	size_t start = mbnum_to_index(statin->input->s, statin->charsoff);

	if (input_width - statin->charsoff > free_space)
		len = width_to_length(statin->input->s + start, free_space);
	else
		len = statin->input->len - start;

	set_cursor_position(0, --window.y);

	clear_line();
	write_console((unsigned char *)msg, msglen);
	write_console(statin->input->s + start, len);

	set_cursor_position(statin->cx + msglen - statin->charsoff, window.y);
}

void status_realloc(size_t len)
{
	if (len >= femto.file.status.cap)
	{
		while (len >= femto.file.status.cap)
			femto.file.status.cap += BUFF_SIZE;
		femto.file.status.msg = (char *)realloc(femto.file.status.msg,
			femto.file.status.cap);
		if (femto.file.status.msg == NULL)
			die("realloc");
	}
	fill_with_spaces(femto.file.status.msg, femto.file.status.cap);
}

void status_set_default(size_t len)
{
	status_realloc(len);
	size_t pathlen = strlen(femto.file.path);
	memmove(femto.file.status.msg, femto.file.path, pathlen);
	if (femto.file.path[0] != '\0' && femto.file.is_modified)
		femto.file.status.msg[strlen(femto.file.path)] = '*';
	char *x = size_t_to_str(femto.file.cursor.x + 1);
	char *y = size_t_to_str(femto.file.cursor.y + 1);
	size_t xlen = strlen(x);
	size_t ylen = strlen(y);
	memmove(femto.file.status.msg + (len - 1) - (ylen + xlen + 1), y, ylen);
	memmove(femto.file.status.msg + (len - 1) - (xlen + 1), ":", 1);
	memmove(femto.file.status.msg + (len - 1) - xlen, x, xlen);
	femto.file.status.msg[len] = '\0';
	femto.file.status.len = len - 1;
	free(x);
	free(y);
}

void status_do_insert(unsigned char *s)
{
	line_insert_str(statin->input,
		mbnum_to_index(statin->input->s, statin->cx++), s);
}

void status_do_home(void)
{
	statin->cx = 0;
}

void status_do_end(void)
{
	statin->cx = num_of_mbchars(statin->input);
}

void status_do_arrow_left(void)
{
	if (statin->cx > 0)
		--statin->cx;
}

void status_do_arrow_right(void)
{
	if (statin->cx < num_of_mbchars(statin->input))
		++statin->cx;
}

void status_do_backspace(void)
{
	if (statin->input->len > 0 && statin->cx > 0)
	{
		size_t charlen = 1;
		size_t i = mbnum_to_index(statin->input->s, statin->cx);
		while (is_continuation_byte(statin->input->s[--i]))
			++charlen;
		line_delete_str(statin->input, i, charlen);
		--statin->cx;
		if (statin->charsoff > 0)
			--statin->charsoff;
	}
}

void status_input_init(void)
{
	statin = (Status_input *)malloc(sizeof(Status_input));
	statin->cx = 0;
	statin->charsoff = 0;
}

void status_input_free(void)
{
	free(statin);
}
