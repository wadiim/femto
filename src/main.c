#include "femto.h"

int main(int argc, char **argv)
{
	init(argc, argv);
	for (;;)
	{
		refresh_screen();
		process_keypress();
	}
}
