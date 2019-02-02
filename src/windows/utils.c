#include <windows.h>
#include <stdbool.h>

bool is_file_exist(char *filename)
{
	DWORD dwAttrib = GetFileAttributes(filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool is_continuation_byte(unsigned char c)
{
	return false;
}
