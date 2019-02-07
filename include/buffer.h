#ifndef BUFFER_H
#define BUFFER_H

typedef struct line Line;

typedef struct
{
	Line *curr;
	Line *begin;
} Buffer;

void buffer_init(Buffer *buffer);
void buffer_free(Buffer *buffer);

#endif
