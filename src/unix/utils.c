#include <sys/stat.h>
#include "utils.h"

bool is_file_exist(char *filename)
{
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

bool is_continuation_byte(unsigned char c)
{
	return (utf8_len(c) == UTF8_CONTINUATION_BYTE);
}
