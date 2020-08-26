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

// File get_info.c
extern Dc20InfoPtr get_info(int fd);

// File get_pic.c
extern int get_pic(int fd, int which, unsigned char *pic, int low_res);

// File get_thumb.c
extern int get_thumb(int fd, int which, unsigned char *thumb);

// File hash_mark.c
extern void hash_init(void);
extern void hash_mark(int in, int total, int range);

// File init_dc20.c
extern void close_dc20(int fd);
extern int init_dc20(char *device, speed_t speed);

// File read_data.c
extern int read_data(int fd, unsigned char *buf, int sz);
extern int end_of_data(int fd);

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
