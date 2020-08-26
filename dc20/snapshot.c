#include <stdio.h>

#include "dc20.h"

static unsigned char snap_pck[] =
	{0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

int snapshot(int fd)
{
  char r;

  if (send_pck(fd, snap_pck) == -1) {
    perror("send_pck");
    return(0);
  }

  if (wait_till_ready( fd ) == -1) {
    perror("wait_till_ready");
    return(0);
  }

  return 1;
}
