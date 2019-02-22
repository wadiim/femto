#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>
#include "chars.h"
#include "utils.h"

size_t utf8_len(unsigned char c)
{
	if ((c & (1 << 7)) == 0)
		return 1;
	if ((c & (1 << 6)) == 0)
		return UTF8_CONTINUATION_BYTE;
	if ((c & (1 << 5)) == 0)
		return 2;
	if ((c & (1 << 4)) == 0)
		return 3;
	return 4;
}

bool is_alnum_mbchar(const unsigned char *s)
{
	wchar_t wc;
	if (mbtowc(&wc, (char *)s, MAXCHARLEN) < 0)
			return 0;
	return iswalnum(wc);
}

size_t move_mbleft(const unsigned char *s, size_t pos)
{
	while (pos && is_continuation_byte(s[--pos]))
		;
	return pos;
}

size_t move_mbright(const unsigned char *s, size_t pos)
{
	while (s[++pos] && is_continuation_byte(s[pos]))
		;
	return pos;
}

size_t index_to_mbnum(const unsigned char *s, size_t n)
{
	size_t num = 0;
	for (size_t i = 0; i < n; ++i)
	{
		if (!is_continuation_byte(s[i]))
			++num;
	}
	return num;
}

size_t mbnum_to_index(const unsigned char *s, size_t n)
{
	size_t pos = 0;
	for (size_t i = 0; i < n; ++i)
	{
		if (!is_continuation_byte(s[pos]))
			++pos;
		while (is_continuation_byte(s[pos]))
			++pos;
	}
	return pos;
}
