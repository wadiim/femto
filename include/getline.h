#ifndef GETLINE_H
#define GETLINE_H

#include <stdint.h>
#include <stdio.h>

typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#endif
