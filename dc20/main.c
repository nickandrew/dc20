#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "dc20.h"

static unsigned char init_pck[] =
	{0x41, 0x00, 0x11, 0x52, 0x00, 0x00, 0x00, 0x1A};

#define USAGE "Usage: dc20bin [-i] [-e] [-a] [-p] [-s #] device \n\n\
\t -a        : Get all snapshots\n\
\t -e        : Erase memory\n\
\t -i        : Get information\n\
\t -p        : Take a single snapshot\n\
\t -r        : Toggle resolution\n\
\t -s #      : Download stored image\n\
\t -t #      : Grab thumbnail\n\
\t device    : Serial port camera is attached to"

int main(int argc, char *argv[] )
{
  int do_info = 0;
  int do_snaps = 0;
  int do_thumbs = 0;
  int i, n;
  int tfd;
  struct termios tty_orig;
  struct termios tty_new;
  Dc20InfoPtr dc20_info;
  
  if ( argc < 2 || strcmp(argv[1],"--help") == 0 ) {
    puts(USAGE);
    exit(0);
  }

  if ((tfd = init_dc20(argv[argc-1], B115200)) == -1) {
    printf("Error: unable to init camera\n");
    exit(-1);
  }
  
  if ((dc20_info = get_info(tfd)) == NULL) {
    printf("Error: could not get info.\n");
    close_dc20(tfd);
    exit(-1);
  }
  
  for ( i=1; i<argc-1; i++ ) {
    switch (argv[i][1]) {

    case 'i': 
      printf("Camera model %x\n", dc20_info->model);
      printf("Version %d.%d\n", dc20_info->ver_major, dc20_info->ver_minor);
      printf("Pictures %d/%d\n", dc20_info->pic_taken,
	     dc20_info->pic_taken + dc20_info->pic_left);
      printf("Resolution is %s\n",
	     (dc20_info->flags.low_res) ? "low" : "high");
      printf("Battery is %s\n",
	     (dc20_info->flags.low_batt) ? "low" : "good");
      break;

    case 't': /* Grab all thumbnails */
      i++;
      if ( i < argc && sscanf( argv[i],"%d",&n ) == 1 ) {
	thumb_to_file(tfd, n);
      } else {
	puts (USAGE);
      }
      break;

    case 'r': /* Toggle resolution */
      close_dc20(tfd);
      tfd = init_dc20(argv[argc-1], B9600);
      toggle_res(tfd, dc20_info);
      break;

    case 'a': /* Grab all snapshots */ 
      pics_to_file(tfd, dc20_info->pic_taken, dc20_info->flags.low_res);
      break; 

    case 'e': /* delete all snapshots */ 
      close_dc20(tfd);
      tfd = init_dc20(argv[argc-1], B9600);
      erasemem(tfd);
      break; 

    case 'p': /* take a single snapshot */

      /*
       * For some reason, I need to go back to 9600 baud
       */
      close_dc20(tfd); 
      tfd = init_dc20(argv[argc-1], B9600);
      snapshot(tfd);
      break; 

    case 's': 
      i++;
      if ( sscanf( argv[i],"%d",&n ) == 1 ) {
	pic_to_file(tfd, n, dc20_info->flags.low_res);
      } else {
	puts (USAGE);
      }
      break; 

    default:
      puts(USAGE);
      break;
    }
  }

  close_dc20(tfd);
  exit(0);
}
