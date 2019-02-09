#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#define UTF8_CONTINUATION_BYTE 0

typedef struct line Line;

void fix_cursor_x(void);
size_t get_tabsize(void);
size_t utf8_len(unsigned char c);
char *size_t_to_str(size_t num);
size_t line_width(Line *line);
bool is_file_exist(char *filename);
size_t num_of_mbchars(Line *line);
bool is_continuation_byte(unsigned char c);
void fill_with_spaces(char *s, size_t len);
size_t find_first_nonblank(const unsigned char *s);
size_t length_to_width(const unsigned char *s, size_t len);
size_t str_width(const unsigned char *s, size_t len);
size_t mbnum_to_index(const unsigned char *s, size_t n);
size_t width_to_length(const unsigned char *s, size_t width);

#endif
