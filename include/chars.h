#ifndef CHARS_H
#define CHARS_H

#include <stdbool.h>
#include <stddef.h>

#define UTF8_CONTINUATION_BYTE 0
#define MAXCHARLEN 6

size_t utf8_len(unsigned char c);
bool is_alnum_mbchar(const unsigned char *s);
size_t move_mbleft(const unsigned char *s, size_t pos);
size_t move_mbright(const unsigned char *s, size_t pos);
size_t index_to_mbnum(const unsigned char *s, size_t n);
size_t mbnum_to_index(const unsigned char *s, size_t n);

#endif
