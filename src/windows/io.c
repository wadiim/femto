#include <windows.h>
#include <stdio.h>
#include "global.h"
#include "utils.h"
#include "term.h"
#include "die.h"
#include "io.h"

wchar_t get_char(int *special_key);

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
		if (input.Event.KeyEvent.wVirtualKeyCode == '\0')
			continue;
	} while (!(input.EventType == KEY_EVENT &&
		input.Event.KeyEvent.bKeyDown));
	if (input.Event.KeyEvent.wVirtualKeyCode == 8 ||
		input.Event.KeyEvent.wVirtualKeyCode == 9 ||
		input.Event.KeyEvent.wVirtualKeyCode == 13 ||
		input.Event.KeyEvent.wVirtualKeyCode == 27 ||
		input.Event.KeyEvent.wVirtualKeyCode >= PAGE_UP &&
		input.Event.KeyEvent.wVirtualKeyCode <= DEL ||
		input.Event.KeyEvent.wVirtualKeyCode >= F_KEY(1) &&
		input.Event.KeyEvent.wVirtualKeyCode <= F_KEY(12))
	{
		*special_key = input.Event.KeyEvent.wVirtualKeyCode;
	}
	else if (input.Event.KeyEvent.uChar.UnicodeChar >= CTRL_KEY('a') &&
		input.Event.KeyEvent.uChar.UnicodeChar <= CTRL_KEY('z'))
	{
		*special_key = input.Event.KeyEvent.uChar.UnicodeChar;
	}
	else
	{
		special_key = '\0';
		return input.Event.KeyEvent.uChar.UnicodeChar;
	}
	return '\0';
}
