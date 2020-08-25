#include "dc20.h"

#define NULL 0L

static unsigned char snap_pck[] =
	{0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

snapshot(fd)
int fd;
{
  char r;

  if (send_pck(fd, snap_pck) == -1) {
    perror("send_pck");
    return(NULL);
  }

  if (wait_till_ready( fd ) == -1) {
    perror("wait_till_ready");
    return(NULL);
  }
  
}

