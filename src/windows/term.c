#include <windows.h>
#include "global.h"
#include "term.h"
#include "die.h"

#ifndef PATH_MAX
# define PATH_MAX 260
#endif

HANDLE hIn, hOut, old_hOut;
TCHAR old_title[PATH_MAX];
DWORD mode;

void restore_title(void);
HWND get_console_hwnd(void);
void save_current_title(void);
void disable_window_resizing(void);
void string_to_tchar_array(const char *s, TCHAR t[]);

void term_init(void)
{
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn == INVALID_HANDLE_VALUE)
		die("GetStdHandle");

	switch_to_alternate_buffer();
	disable_window_resizing();
	save_current_title();
	set_title("Femto");
	enable_raw_mode();
}

void get_window_size(size_t *x, size_t *y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hOut, &csbi))
		die("GetConsoleScreenBufferInfo");
	if (x != NULL)
        	*x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	if (y != NULL)
        	*y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void clear_line(void)
{
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
		die("GetConsoleScreenBufferInfo");
	COORD coordScreen = { 0, csbi.dwCursorPosition.Y };
	if (!FillConsoleOutputCharacter(hOut, (TCHAR)' ',
		csbi.dwSize.X, coordScreen, &cCharsWritten))
		die("FillConsoleOutputCharacter");
	if (!FillConsoleOutputAttribute(hOut, csbi.wAttributes,
		csbi.dwSize.X, coordScreen, &cCharsWritten))
		die("FillConsoleOutputAttribute");
	if (!SetConsoleCursorPosition(hOut, coordScreen))
		die("SetConsoleCursorPosition");
}

void clear_screen(void)
{
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten, dwConSize;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
		die("GetConsoleScreenBufferInfo");
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	if (!FillConsoleOutputCharacter(hOut, (TCHAR)' ',
		dwConSize, coordScreen, &cCharsWritten))
		die("FillConsoleOutputCharacter");
	if (!FillConsoleOutputAttribute(hOut, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten))
		die("FillConsoleOutputAttribute");
	if (!SetConsoleCursorPosition(hOut, coordScreen))
		die("SetConsoleCursorPosition");
}

void set_title(const char *title)
{
	TCHAR new_title[MAX_PATH];
	string_to_tchar_array(title, new_title);
	if (!SetConsoleTitle(new_title))
		die("SetConsoleTitle");
	atexit(restore_title);
}

void string_to_tchar_array(const char *s, TCHAR t[])
{
	while (*t++ = *s++);
}

void save_current_title(void)
{
	if (!GetConsoleTitle(old_title, MAX_PATH))
		die("GetConsoleTitle");
}

void restore_title(void)
{
	if (!SetConsoleTitle(old_title))
		die("SetConsoleTitle");
}

void disable_raw_mode(void)
{
	if (!SetConsoleMode(hIn, mode))
		die("SetConsoleMode");
}

void enable_raw_mode(void)
{
	atexit(disable_raw_mode);
	if (!GetConsoleMode(hIn, &mode))
		die("GetConsoleMode");
	if (!(SetConsoleMode(hIn, mode & ~(ENABLE_LINE_INPUT |
		ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT |
		ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT))))
		die("SetConsoleMode");
}

void get_buffer_size(size_t *x, size_t *y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
		die("GetConsoleScreenBufferInfo");
	if (x != NULL)
		*x = csbi.dwSize.X;
	if (y != NULL)
		*y = csbi.dwSize.Y;
}

void switch_to_normal_buffer(void)
{
	if (!SetConsoleActiveScreenBuffer(old_hOut))
		die("SetConsoleActiveScreenBuffer");
}

void switch_to_alternate_buffer(void)
{
	old_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hOut = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);
	if (old_hOut == INVALID_HANDLE_VALUE ||
		hOut == INVALID_HANDLE_VALUE)
		die("CreateConsoleScreenBuffer");
	if (!SetConsoleActiveScreenBuffer(hOut))
		die("SetConsoleActiveScreenBuffer");
	atexit(switch_to_normal_buffer);
}

void hide_cursor(void)
{
	CONSOLE_CURSOR_INFO cursorInfo;
	if (!GetConsoleCursorInfo(hOut, &cursorInfo))
		die("GetConsoleCursorInfo");
	cursorInfo.bVisible = 0;
	if (!SetConsoleCursorInfo(hOut, &cursorInfo))
		die("SetConsoleCursorInfo");
}

void show_cursor(void)
{
	CONSOLE_CURSOR_INFO cursorInfo;
	if (!GetConsoleCursorInfo(hOut, &cursorInfo))
		die("GetConsoleCursorInfo");
	cursorInfo.bVisible = 1;
	if (!SetConsoleCursorInfo(hOut, &cursorInfo))
		die("SetConsoleCursorInfo");
}

void set_cursor_position(size_t x, size_t y)
{
	COORD coord;
	coord.X = (SHORT)x;
	coord.Y = (SHORT)y;
	if (!SetConsoleCursorPosition(hOut, coord))
		die("SetConsoleCursorPosition");
}

void get_cursor_position(size_t *x, size_t *y)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (!GetConsoleScreenBufferInfo(hOut, &cbsi))
		die("GetConsoleScreenBufferInfo");
	if (x != NULL)
		*x = cbsi.dwCursorPosition.X;
	if (y != NULL)
		*y = cbsi.dwCursorPosition.Y;
}

void disable_window_resizing(void)
{
	long dwStyle;
	HWND hwnd = get_console_hwnd();
	if (!(dwStyle = GetWindowLong(hwnd, GWL_STYLE)))
		die("GetWindowLong");
	dwStyle ^= WS_THICKFRAME;
	if (!SetWindowLong(hwnd, GWL_STYLE, dwStyle))
		die("SetWindowLong");
}

HWND get_console_hwnd(void)
{
	HWND hwnd;
	TCHAR title[MAX_PATH];
	if (!GetConsoleTitle(title, MAX_PATH))
		die("GetConsoleTitle");
	if (!(hwnd = FindWindow(NULL, title)))
		die("FindWindow");
	return hwnd;
}
