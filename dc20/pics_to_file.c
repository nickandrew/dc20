#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAGIC "COMET"

pics_to_file(tfd, n, low_res)
     int tfd;
     int n;
     int low_res;
{
 int i;
 
 for (i = 0; i < n; i++) {
   pic_to_file(tfd, i + 1, low_res);
 }
}
   

pic_to_file(tfd, n, low_res)
     int tfd;
     int n;
     int low_res;
{ int ofd;
 unsigned char pic[124928];
 char file[1024];
 char header[256];
 int sz = ((low_res) ? 61 : 122)*1024;
 int lsz = ((low_res) ? 256 : 512);
 int rsz = ((low_res) ? 256  - 6 : 512 - 11);
 int off;
 int i, j;
 
 if (get_pic(tfd, n, pic, low_res) == -1)
   return(-1);
 
 sprintf(file, "pic_%d.cmt", n );
   
 if ((ofd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
   perror("open");
   return(-1);
 }
   
 if (write(ofd, MAGIC, sizeof(MAGIC)) != sizeof(MAGIC)) {
   perror("write");
   close(ofd);
   return(-1);
 }
   
 if (lseek(ofd, 128, SEEK_SET) == -1) {
   perror("lseek");
   close(ofd);
     return(-1);
 }
   
 if (write(ofd, (char *)pic, sz) != sz) {
   perror("write");
   close(ofd);
   return(-1);
 }
 
 close(ofd);
 
 return(0);
}
