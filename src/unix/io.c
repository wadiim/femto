#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "utils.h"
#include "die.h"
#include "io.h"

int get_char(void *c);
int parse_input(unsigned char *s);
int parse_sequence(unsigned char *s);
int parse_escape_sequence(void);

void write_console(const unsigned char *s, size_t len)
{
	if (write(STDOUT_FILENO, s, len) == -1)
		die("write");
}

void writeln_console(const unsigned char *s, size_t len)
{
	write_console(s, len);
	write_console((unsigned char *)"\n\r", 2);
}

size_t read_console(unsigned char **s, int *special_key)
{
	unsigned char input;
	while (get_char(&input) != 1)
		;
	size_t len = utf8_len(input);
	if (!(*s = (unsigned char *)malloc(len + 1)))
		die("realloc");
	(*s)[0] = input;
	size_t size = 1;
	for (size_t i = 1; i < len && get_char(&input) == 1; ++i, ++size)
		(*s)[i] = input;
	(*s)[size] = '\0';
	*special_key = parse_input(*s);
	return *special_key ? 0 : size;
}

int get_char(void *c)
{
	int nread;
	nread = read(STDIN_FILENO, c, 1);
	if (nread == -1 && errno != EAGAIN)
		die("read");
	return nread;
}

int parse_input(unsigned char *s)
{
	return (s[0] == ESC) ? parse_escape_sequence() : parse_sequence(s);
}

int parse_sequence(unsigned char *s)
{
	if (s[0] == '\x7f')
		return BACKSPACE;
	else if (s[0] == TAB)
		return TAB;
	else if (s[0] == ENTER)
		return ENTER;
	else if (s[0] >= CTRL_KEY('a') && s[0] <= CTRL_KEY('z'))
		return CTRL_KEY(s[0]);
	else
		return 0;
}

int parse_escape_sequence(void)
{
	unsigned char buf[4] = { 0 };

	if (get_char(&buf[0]) == -1)
		return ESC;
	if (get_char(&buf[1]) == -1)
		return ESC;

	switch (buf[0])
	{
	case '[':
		if (buf[1] >= '0' && buf[1] <= '9')
		{
			if (get_char(&buf[2]) == -1)
				return ESC;

			if (buf[2] == '~')
			{
				switch (buf[1])
				{
				case '1':
					return HOME;
				case '2':
					return INSERT;
				case '3':
					return DEL;
				case '4':
					return END;
				case '5':
					return PAGE_UP;
				case '6':
					return PAGE_DOWN;
				case '7':
					return HOME;
				case '8':
					return END;
				}
			}
			else if (buf[2] >= '0' && buf[2] <= '9')
			{
				if (get_char(&buf[3]) == -1)
					return ESC;

				if (buf[3] == '~')
				{
					switch (buf[1])
					{
					case '1':
						switch (buf[2])
						{
						case '5':
							return F_KEY(5);
						case '7':
							return F_KEY(6);
						case '8':
							return F_KEY(7);
						case '9':
							return F_KEY(8);
						}
					case '2':
						switch (buf[2])
						{
						case '0':
							return F_KEY(9);
						case '1':
							return F_KEY(10);
						case '3':
							return F_KEY(11);
						case '4':
							return F_KEY(12);
						}
					}
				}
			}
			else if (buf[2] == ';')
			{
				if (get_char(&buf[3]) == -1)
					return ESC;
				if (get_char(&buf[4]) == -1)
					return ESC;

				if (buf[3] == '5')
				{
					switch(buf[4])
					{
					case 'A':
						return CTRL_ARROW_UP;
					case 'B':
						return CTRL_ARROW_DOWN;
					case 'C':
						return CTRL_ARROW_RIGHT;
					case 'D':
						return CTRL_ARROW_LEFT;
					}
				}
			}
		}
		else
		{
			switch (buf[1])
			{
			case 'A':
				return ARROW_UP;
			case 'B':
				return ARROW_DOWN;
			case 'C':
				return ARROW_RIGHT;
			case 'D':
				return ARROW_LEFT;
			case 'F':
				return END;
			case 'H':
				return HOME;
			}
		}
	case 'O':
		switch (buf[1])
		{
		case 'A':
			return ARROW_UP;
		case 'B':
			return ARROW_DOWN;
		case 'C':
			return ARROW_LEFT;
		case 'D':
			return ARROW_RIGHT;
		case 'P':
			return F_KEY(1);
		case 'Q':
			return F_KEY(2);
		case 'R':
			return F_KEY(3);
		case 'S':
			return F_KEY(4);
		}
	}
	return ESC;
}
