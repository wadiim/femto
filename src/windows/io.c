#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include "global.h"
#include "utils.h"
#include "term.h"
#include "die.h"
#include "io.h"

wchar_t get_char(int *special_key);
bool is_ctrl_char(INPUT_RECORD *ir);
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

	WORD key_code = input.Event.KeyEvent.wVirtualKeyCode;

	if (key_code == BACKSPACE || key_code == TAB || key_code == ENTER ||
		key_code == ESC || key_code == DEL || key_code == PAGE_UP ||
		key_code == PAGE_DOWN || key_code == SELECT ||
		key_code == PRINT || key_code == EXECUTE ||
		key_code == PRINT_SCREEN || key_code == INSERT ||
		key_code >= F_KEY(1) && key_code <= F_KEY(12))
	{
		*special_key = key_code;
	}
	else if (key_code == HOME || key_code == END ||
		key_code == ARROW_LEFT || key_code == ARROW_UP ||
		key_code == ARROW_RIGHT || key_code == ARROW_DOWN)
	{
		if (is_ctrl_pressed(&input))
		{
			switch (key_code)
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
			}
		}
		else
			*special_key = key_code;
	}
	else if (is_ctrl_char(&input))
		*special_key = input.Event.KeyEvent.uChar.UnicodeChar;
	else
	{
		special_key = '\0';
		return input.Event.KeyEvent.uChar.UnicodeChar;
	}
	return '\0';
}

bool is_ctrl_pressed(INPUT_RECORD *ir)
{
	return ir->Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED ||
		ir->Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED;
}

bool is_ctrl_char(INPUT_RECORD *ir)
{
	return ir->Event.KeyEvent.uChar.UnicodeChar >= CTRL_KEY('a') &&
		ir->Event.KeyEvent.uChar.UnicodeChar <= CTRL_KEY('z');
}
