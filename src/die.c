#include <stdlib.h>
#include <stdio.h>
#include "term.h"

void die(const char *msg)
{
	switch_to_normal_buffer();
	perror(msg);
	printf("\r");
	exit(0);
}
