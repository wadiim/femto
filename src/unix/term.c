#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "femto.h"
#include "term.h"
#include "die.h"
#include "io.h"

struct termios orig_termios;
struct sigaction sa;

void set_sigaction(void);
void sigaction_init(void);
void handle_signal(int signal);
void create_signal_handler(void);
void delete_signal_handler(void);

void term_init(void)
{
	set_title("femto");
	enable_raw_mode();
	sigaction_init();
	create_signal_handler();
	switch_to_alternate_buffer();
}

void get_window_size(size_t *x, size_t *y)
{
	struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        {
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			die("write");
		get_cursor_position(x, y);
		if (x != NULL)
			++(*x);
		if (y != NULL)
			++(*y);
	}
        else
        {
		if (x != NULL)
			*x = ws.ws_col;
		if (y != NULL)
			*y = ws.ws_row;
        }
}

void clear_line(void)
{
	if (write(STDOUT_FILENO, "\x1b[2K", 4) != 4)
		die("write");
}

void clear_screen(void)
{
	if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4)
		die("write");
	if (write(STDOUT_FILENO, "\x1b[H", 3) != 3)
		die("write");
}

void set_title(const char *title)
{
	char buff[64] = "\0";
	if (sprintf(buff, "\x1b]0;%s\x7", title) < 0)
		die("sprintf");
	if (write(STDOUT_FILENO, buff, sizeof(buff)) == -1)
		die("write");
}

void disable_raw_mode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enable_raw_mode(void)
{
	struct termios raw = orig_termios;
	atexit(disable_raw_mode);
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

void get_buffer_size(size_t *x, size_t *y)
{
	get_window_size(x, y);
}

void switch_to_normal_buffer(void)
{
	if (write(STDOUT_FILENO, "\x1b[?1049l", 8) != 8)
		die("write");
}

void switch_to_alternate_buffer(void)
{
	if (write(STDOUT_FILENO, "\x1b[?1049h\x1b[H", 11) != 11)
		die("write");
	atexit(switch_to_normal_buffer);
}

void hide_cursor(void)
{
	write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void show_cursor(void)
{
	write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void set_cursor_position(size_t x, size_t y)
{
	char buff[32] = "\0";
	if (sprintf(buff, "\x1b[%zu;%zuH", y + 1, x + 1) < 0)
		die("sprintf");
	if (write(STDOUT_FILENO, buff, sizeof(buff)) == -1)
		die("write");
}

void get_cursor_position(size_t *x, size_t *y)
{
	char buf[32];
	unsigned i = 0;
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		die("write");
	while (i < sizeof(buf) - 1)
	{
		if (read(STDIN_FILENO, &buf[i], 1) != 1)
			break;
		if (buf[i] == 'R')
			break;
		++i;
	}
	buf[i] = '\0';
	if (buf[0] != '\x1b' || buf[1] != '[')
		die("get_cursor_position");
	if (x != NULL)
	{
		if (sscanf(&buf[4], "%zu", x) != 1)
			die("sscanf");
		--(*x);
	}
	if (y != NULL)
	{
		if (sscanf(&buf[2], "%zu", y) != 1)
			die("sscanf");
		--(*y);
	}
}

void sigaction_init(void)
{
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);
}

void handle_signal(int signal)
{
	switch (signal)
	{
	case SIGWINCH:
    		refresh_screen();
    		break;
	}
}

void create_signal_handler(void)
{
	sa.sa_handler = &handle_signal;
	set_sigaction();
}

void delete_signal_handler(void)
{
	sa.sa_handler = SIG_DFL;
	set_sigaction();
}

void set_sigaction(void)
{
	if (sigaction(SIGWINCH, &sa, NULL) == -1)
		die("Cannot handle SIGWINCH");
}
