#include <termios.h>

typedef struct dc20_info_s {
  unsigned char model;
  unsigned char ver_major;
  unsigned char ver_minor;
  int pic_taken;
  int pic_left;
  struct {
    unsigned int low_res:1;
    unsigned int low_batt:1;
  } flags;
} Dc20Info, *Dc20InfoPtr;

Dc20Info *get_info(int);

// File init_dc20.c
extern void close_dc20(int fd);
extern int init_dc20(char *device, speed_t speed);

// File send_pck.c
extern int send_pck(int fd, unsigned char *pck);
