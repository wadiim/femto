#include "buffer.h"
#include "line.h"

void buffer_init(Buffer *buffer)
{
	buffer->begin = buffer->curr = line_insert(NULL, NULL);
}

void buffer_free(Buffer *buffer)
{
	while (buffer->begin != NULL)
	{
		Line *temp = buffer->begin;
		buffer->begin = buffer->begin->next;
		line_delete(temp);
	}
}
