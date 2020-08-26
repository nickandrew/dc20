#include <stdio.h>

#include "dc20.h"

static int oldhash;

void hash_init(void)
{
	oldhash = 0;
}

void hash_mark(int in, int total, int range)
{
	int percent = (in) ? (total*100)/in : 0;
	int h = (percent) ? (range*100)/percent : 0;

	while (oldhash < h) {
		printf("#");
		fflush(stdout);
		oldhash++;
	}
}
