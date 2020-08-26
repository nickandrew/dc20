#include <stdio.h>

#include "dc20.h"

static unsigned char pck[] =
	{0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};

int toggle_res(int fd, Dc20InfoPtr dc20_info)
{
  char r;

  pck[2] = (dc20_info->flags.low_res) ? 0 : 1;
  
  if (send_pck(fd, pck) == -1) {
    printf("Error: could not change resolution.\n");
    perror("send_pck");
    return 0;
  }
  return 1;
}
