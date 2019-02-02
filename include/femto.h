#ifndef FEMTO_H
#define FEMTO_H

#include "file.h"

#define SCREEN_ROWS(x) (((x) <= 1) ? 1 : ((x) - 1))
#define LINE_ROWS(l, x) (((l) / (x)) + (((l) % (x) != 0) ? 1 : 0))

typedef Cursor Window;

struct
{
	File file;
	size_t tabsize;
} femto;

void init(int argc, char **argv);
void refresh_screen(void);
void process_keypress(void);

#endif
