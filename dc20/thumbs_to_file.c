#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dc20.h"

#define HEADER "P5\n80 60\n255\n"

thumb_to_file(tfd, n)
int tfd;
int n;
{
	int ofd;
	unsigned char thumb[4800];
	char file[1024];
	char buf[256];
	
	if (get_thumb(tfd, n, thumb) == -1)
	  return(-1);
	
	sprintf(file, "thumb_%d.pgm", n );

	if ((ofd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
	  perror("open");
	  return(-1);
	}

	if (write(ofd, HEADER, strlen(HEADER)) != strlen(HEADER)) {
	  perror("write");
	  close(ofd);
	  return(-1);
	}

	if (write(ofd, (char *)thumb, 4800) != 4800) {
	  perror("write");
	  close(ofd);
	  return(-1);
	}
	
	close(ofd);

	return(0);
}

thumbs_to_file(tfd, n)
int tfd;
int n;
{
  int i;
  
  for (i = 0; i < n; i++) 
    thumb_to_file(tfd, i + 1);
}

