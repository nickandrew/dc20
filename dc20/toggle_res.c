#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include "dc20.h"

#define DC20TTY "/dev/ttyR1"

static unsigned char res_pck[] =
	{0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

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
		printf("Battery is %s\n",
			(dc20_info->flags.low_batt) ? "low" : "good");

		if (dc20_info->pic_taken) {
			fprintf(stderr, "Can't switch res with pictures in memory.\n");
			exit(-1);
		}

		printf("Switching to %s res mode\n",
			(dc20_info->flags.low_res) ? "high" : "low");
		res_pck[2] = (dc20_info->flags.low_res) ? 0 : 1;

		if (send_pck(tfd, res_pck) == -1) {
			fprintf(stderr, "Could not change res.\n");
			exit(-1);
		}

	}
	else
		printf("Could not get info.\n");


	close_dc20(tfd);
	exit(0);
}
