#include <stdio.h>

#include "dc20.h"

static unsigned char erase_pck[] =
	{0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

int erasemem(int fd)
{
  if (send_pck(fd, erase_pck) == -1) { 
    perror("send_pck");
    return(0);
  }
  
  if (wait_till_ready( fd ) == -1) {
    perror("wait_till_ready");
    return(0);
  }

  return 1;
}
