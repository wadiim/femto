#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>
#include <stdlib.h>

enum { NORMAL, INPUT, MESSAGE } status_mode;

typedef struct line Line;

typedef struct status
{
	char *msg;
	size_t len;
	size_t cap;
} Status;

typedef struct status_input
{
	size_t cx;
	char *msg;
	Line *input;
	size_t charsoff;
} Status_input;

void status_init(void);
void status_free(void);

void status(const char *msg);
void status_print(size_t cols);

bool status_input(Line *input, char *msg, const char *placeholder);

#endif
