#include "dc20.h"

#define NULL 0L

static unsigned char info_pck[] =
	{0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

Dc20InfoPtr
get_info(fd)
int fd;
{
	static Dc20Info result;
	unsigned char buf[256];

	if (send_pck(fd, info_pck) == -1) {
		perror("write");
		return(NULL);
	}

	printf("Read info packet.\n");
	if (read_data(fd, buf, 256) == -1)
		return(NULL);

	if (end_of_data(fd) == -1)
		return(NULL);

	result.ver_major = buf[2];
	result.ver_minor = buf[3];
	result.pic_taken = buf[8]<<8|buf[9];
	result.pic_left = buf[10]<<8|buf[11];
	result.flags.low_res = buf[23];
	result.flags.low_batt = buf[29];
	return(&result);
}
