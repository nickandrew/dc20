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

// File erasemem.c
extern int erasemem(int fd);

// File init_dc20.c
extern void close_dc20(int fd);
extern int init_dc20(char *device, speed_t speed);

// File send_pck.c
extern int send_pck(int fd, unsigned char *pck);

// File pics_to_file.c
extern int pic_to_file(int tfd, int n, int low_res);
extern void pics_to_file(int tfd, int n, int low_res);

// File snapshot.c
extern int snapshot(int fd);

// File thumbs_to_file.c
extern int thumb_to_file(int tfd, int n);
extern void thumbs_to_file(int tfd, int n);

// File toggle_res.c
extern int toggle_res(int fd, Dc20InfoPtr dc20_info);
