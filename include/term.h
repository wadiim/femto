#ifndef TERM_H
#define TERM_H

#include <stdlib.h>
#include <stdbool.h>

void term_init(void);
void get_window_size(size_t *x, size_t *y);
void clear_line(void);
void clear_screen(void);
void set_title(const char *title);
void enable_raw_mode(void);
void disable_raw_mode(void);
void get_buffer_size(size_t *x, size_t *y);
void switch_to_normal_buffer(void);
void switch_to_alternate_buffer(void);
void hide_cursor(void);
void show_cursor(void);
void set_cursor_position(size_t x, size_t y);
void get_cursor_position(size_t *x, size_t *y);

#endif
