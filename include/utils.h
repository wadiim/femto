#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

void fix_cursor_x(void);
size_t get_tabsize(void);
char *size_t_to_str(size_t num);
bool is_file_exist(char *filename);
bool is_continuation_byte(unsigned char c);
void fill_with_spaces(char *s, size_t len);
size_t find_first_nonblank(const unsigned char *s);
size_t str_width(const unsigned char *s, size_t len);
size_t length_to_width(const unsigned char *s, size_t len);
size_t width_to_length(const unsigned char *s, size_t width);

#endif
