#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include "dc20.h"

static unsigned char init_pck[] =
	{0x41, 0x00, 0x11, 0x52, 0x00, 0x00, 0x00, 0x1A};

static struct termios tty_orig;

int init_dc20(char *device, speed_t speed)
{
	struct termios tty_new;
	int tfd;

	switch (speed) {
	case B9600:
		init_pck[2] = 0x96;
		init_pck[3] = 0x00;
		break;
	case B19200:
		init_pck[2] = 0x19;
		init_pck[3] = 0x20;
		break;
	case B38400:
		init_pck[2] = 0x38;
		init_pck[3] = 0x40;
		break;
	case B57600:
		init_pck[2] = 0x57;
		init_pck[3] = 0x60;
		break;
	case B115200:
		init_pck[2] = 0x11;
		init_pck[3] = 0x52;
		break;
	default:
		fprintf(stderr, "Unsupported baud rate.\n");
		return(-1);
	}

/*
 Open device file.
*/
	if ((tfd = open(device, O_RDWR)) == -1) {
		perror("open");
		fprintf(stderr, "Could not open %s for read/write.\n", device);
		return(-1);
	}
/*
 Save old device information to restore when we are done.
*/
	if (tcgetattr(tfd, &tty_orig) == -1) {
		perror("tcgetattr");
		return(-1);
	}

	memcpy((char *)&tty_new, (char *)&tty_orig, sizeof(struct termios));
/*
 We need the device to be raw. 8 bits even parity on 9600 baud to start.
*/
	cfmakeraw(&tty_new);
	tty_new.c_oflag &= ~CSTOPB;
	tty_new.c_cflag |= PARENB;
	tty_new.c_cflag &= ~PARODD;
	tty_new.c_cc[VMIN] = 0;
	tty_new.c_cc[VTIME] = 50;
	cfsetospeed(&tty_new, B9600);
	cfsetispeed(&tty_new, B9600);

	if (tcsetattr(tfd, TCSANOW, &tty_new) == -1) {
		perror("tcsetattr");
		return(-1);
	}

	if (send_pck(tfd, init_pck) == -1) {
		tcsetattr(tfd, TCSANOW, &tty_orig);
		return(-1);
	}
/*
 Set speed to requested speed.
*/
	cfsetospeed(&tty_new, speed);
	cfsetispeed(&tty_new, speed);

	if (tcsetattr(tfd, TCSANOW, &tty_new) == -1) {
		perror("tcsetattr");
		return(-1);
	}

	return(tfd);
}

void close_dc20(int fd)
{
  /*
    Set camera back to 9600 baud rate
  */
  init_pck[2] = 0x96;
  init_pck[3] = 0x00;
  send_pck(fd, init_pck);
  
  /*
    Restore original device settings.
  */
  if (tcsetattr(fd, TCSANOW, &tty_orig) == -1)
    perror("tcsetattr");
  
  if (close(fd) == -1)
    perror("close");

}
