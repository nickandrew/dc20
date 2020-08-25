#include <stdio.h>

static unsigned char pic_pck[] =
	{0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

get_pic(fd, which, pic, low_res)
int fd;
int which;
unsigned char *pic;
int low_res;
{
	unsigned char buf[1024];
	int n = (low_res) ? 61 : 122;
	int i;

	pic_pck[3] = (unsigned char)which;

	if (send_pck(fd, pic_pck) == -1)
		return(-1);

	printf("Get image #%d: ", which);
	hash_init();

	for (i = 0; i < n; i++) {
		hash_mark(i, n - 1, 40);

		if (read_data(fd, buf, 1024) == -1)
			return(-1);

		memcpy((char *)&pic[i*1024], buf, 1024);
	}

	printf("\n");
	return(end_of_data(fd));
}
