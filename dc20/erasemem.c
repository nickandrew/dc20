#include "dc20.h"

#define NULL 0L

static unsigned char erase_pck[] =
	{0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

erasemem(fd)
int fd;
{
  if (send_pck(fd, erase_pck) == -1) { 
    perror("send_pck");
    return(NULL);
  }
  
  if (wait_till_ready( fd ) == -1) {
    perror("wait_till_ready");
    return(NULL);
  }
}
