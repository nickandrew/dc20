#include <stdio.h>
#include <string.h>

#include "dc20.h"

static unsigned char thumb_pck[] =
	{0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

get_thumb(fd, which, thumb)
int fd;
int which;
unsigned char *thumb;
{
	unsigned char buf[1024];
	int i;

	thumb_pck[3] = (unsigned char)which;

	if (send_pck(fd, thumb_pck) == -1)
		return(-1);

	printf("Get thumb #%d: ", which);
	fflush(stdout);
	hash_init();

	for (i = 0; i < 5; i++) {
		hash_mark(i, 4, 40);

		if (read_data(fd, buf, 1024) == -1)
			return(-1);

		memcpy((char *)&thumb[i*1024], buf, (i*1024 + 1024 > 4800)? 704 : 1024);
	}

	printf("\n");

	for (i = 0; i < 4800; i++) {

		if (thumb[i] + 50 <= 255)
			thumb[i] += 50;

	}

	return(end_of_data(fd));
}
