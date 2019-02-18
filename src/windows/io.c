#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "global.h"
#include "utils.h"
#include "term.h"
#include "die.h"
#include "io.h"

wchar_t get_char(int *special_key);
bool is_ctrl_pressed(INPUT_RECORD *ir);

void write_console(const unsigned char *s, size_t len)
{
	DWORD cWritten = len;
	if (!WriteConsole(hOut, s, len, &cWritten, NULL))
		die("WriteConsole");
}

void writeln_console(const unsigned char *s, size_t len)
{
	size_t wx;
	get_window_size(&wx, NULL);

	if (str_width(s, len) >= wx)
		write_console(s, len);
	else
	{
		unsigned char *buff = (unsigned char *)malloc(len + 1);
		memmove(buff, s, len);
		memmove(&buff[len], "\n", 1);
		write_console(buff, len + 1);
		free(buff);
	}
}

size_t read_console(unsigned char **s, int *special_key)
{
	wchar_t c = get_char(special_key);
	if (c == '\0')
	{
		if (!(*s = (unsigned char *)malloc(1)))
			die("malloc");
		(*s)[0] = '\0';
		return 0;
	}
	char buff[4] = { '\0' };
	size_t size =
		WideCharToMultiByte(CP_OEMCP, 0, &c, 1, buff, 4, NULL, NULL);
	if (size == 0)
		die("WideCharToMultiByte");
	if (!(*s = (unsigned char *)malloc(size + 1)))
		die("malloc");
	for (size_t i = 0; i < size; ++i)
		(*s)[i] = buff[i];
	(*s)[size] = '\0';
	return size;
}

wchar_t get_char(int *special_key)
{
	INPUT_RECORD input;
	DWORD nread;

	do
	{
		if (!ReadConsoleInputW(hIn, &input, 1, &nread))
			die("ReadConsoleInputW");
	} while (!(input.EventType == KEY_EVENT &&
		input.Event.KeyEvent.bKeyDown &&
		input.Event.KeyEvent.wVirtualKeyCode));

	wchar_t retval = '\0';
	WORD keycode = input.Event.KeyEvent.wVirtualKeyCode;
	WORD unicode = input.Event.KeyEvent.uChar.UnicodeChar;

	if (unicode >= CTRL_KEY('a') && unicode <= CTRL_KEY('z'))
		*special_key = unicode;
	else if (is_ctrl_pressed(&input))
	{
		switch (keycode)
		{
		case END:
			*special_key = CTRL_END;
			break;
		case HOME:
			*special_key = CTRL_HOME;
			break;
		case ARROW_LEFT:
			*special_key = CTRL_ARROW_LEFT;
			break;
		case ARROW_UP:
			*special_key = CTRL_ARROW_UP;
			break;
		case ARROW_RIGHT:
			*special_key = CTRL_ARROW_RIGHT;
			break;
		case ARROW_DOWN:
			*special_key = CTRL_ARROW_DOWN;
			break;
		default:
			*special_key = '\0';
			retval = unicode;
			break;
		}
	}
	else if (keycode >= F_KEY(1) && keycode <= F_KEY(12))
		*special_key = keycode;
	else
	{
		switch (keycode)
		{
		case BACKSPACE:
		case TAB:
		case ENTER:
		case ESC:
		case PAGE_UP:
		case PAGE_DOWN:
		case END:
		case HOME:
		case ARROW_LEFT:
		case ARROW_UP:
		case ARROW_RIGHT:
		case ARROW_DOWN:
		case SELECT:
		case PRINT:
		case EXECUTE:
		case PRINT_SCREEN:
		case INSERT:
		case DEL:
			*special_key = keycode;
			break;
		default:
			*special_key = '\0';
			retval = unicode;
			break;
		}
	}
	return retval;
}

bool is_ctrl_pressed(INPUT_RECORD *ir)
{
	return ir->Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ||
		ir->Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED;
}
