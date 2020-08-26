#include <stdio.h>

#include "dc20.h"

static int oldhash;

hash_init()
{
	oldhash = 0;
}

hash_mark(in, total, range)
int in;
int total;
int range;
{
	int percent = (in) ? (total*100)/in : 0;
	int h = (percent) ? (range*100)/percent : 0;

	while (oldhash < h) {
		printf("#");
		fflush(stdout);
		oldhash++;
	}

}
