#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include "dc20.h"

#define DC20TTY "/dev/ttyR1"

static unsigned char init_pck[] =
	{0x41, 0x00, 0x11, 0x52, 0x00, 0x00, 0x00, 0x1A};

main()
{
	int tfd;
	struct termios tty_orig;
	struct termios tty_new;
	Dc20InfoPtr dc20_info;

	if ((tfd = init_dc20(DC20TTY, B115200)) == -1) {
		exit(-1);
	}

	if ((dc20_info = get_info(tfd)) != NULL) {
		printf("Version %d.%d\n", dc20_info->ver_major, dc20_info->ver_minor);
		printf("Pictures %d/%d\n", dc20_info->pic_taken,
			dc20_info->pic_taken + dc20_info->pic_left);
		printf("Resolution is %s\n",
			(dc20_info->flags.low_res) ? "low" : "high");
		printf("Battery is %s\n",
			(dc20_info->flags.low_batt) ? "low" : "good");
	}
	else
		printf("Could not get info.\n");

	thumbs_to_file(tfd, dc20_info->pic_taken);

	pics_to_file(tfd, dc20_info->pic_taken, dc20_info->flags.low_res);

	close_dc20(tfd);
	exit(0);
}
