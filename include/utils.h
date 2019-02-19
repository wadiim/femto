#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#define UTF8_CONTINUATION_BYTE 0

void fix_cursor_x(void);
size_t get_tabsize(void);
size_t utf8_len(unsigned char c);
char *size_t_to_str(size_t num);
bool is_file_exist(char *filename);
bool is_continuation_byte(unsigned char c);
void fill_with_spaces(char *s, size_t len);
bool is_alnum_mbchar(const unsigned char *s);
size_t find_first_nonblank(const unsigned char *s);
size_t length_to_width(const unsigned char *s, size_t len);
size_t str_width(const unsigned char *s, size_t len);
size_t index_to_mbnum(const unsigned char *s, size_t n);
size_t mbnum_to_index(const unsigned char *s, size_t n);
size_t width_to_length(const unsigned char *s, size_t width);
size_t move_mbleft(const unsigned char *s, size_t pos);
size_t move_mbright(const unsigned char *s, size_t pos);

#endif
