#ifndef IO_H
#define IO_H

#include <stdbool.h>

#define F_KEY(x) ((x) + 0x6f)
#define CTRL_KEY(k) ((k) & 0x1f)

enum key
{
	BACKSPACE = 8,
	TAB,
	ENTER = 13,
	ESC = 27,
	PAGE_UP = 33,
	PAGE_DOWN,
	END,
	HOME,
	ARROW_LEFT,
	ARROW_UP,
	ARROW_RIGHT,
	ARROW_DOWN,
	SELECT,
	PRINT,
	EXECUTE,
	PRINT_SCREEN,
	INSERT,
	DEL,
	CTRL_END,
	CTRL_HOME,
	CTRL_ARROW_LEFT,
	CTRL_ARROW_UP,
	CTRL_ARROW_RIGHT,
	CTRL_ARROW_DOWN
};

void write_console(const unsigned char *s, size_t len);
void writeln_console(const unsigned char *s, size_t len);
size_t read_console(unsigned char **s, int *special_key);

#endif
