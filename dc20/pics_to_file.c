#include <unistd.h>
#include <fcntl.h>

#define MAGIC "COMET"

pics_to_file(tfd, n, low_res)
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

	for (i = 0; i < n; i++) {

		if (get_pic(tfd, i + 1, pic, low_res) == -1)
			return(-1);

		sprintf(file, "pic_%d.cmt", i + 1);

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
		sprintf(file, "pic_%d.pgm", i + 1);
		sprintf(header, "P5\n%d 242\n255\n", rsz);

		if ((ofd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
			perror("open");
			return(-1);
		}

		if (write(ofd, header, strlen(header)) != strlen(header)) {
			perror("write");
			close(ofd);
			return(-1);
		}

		for (j = 1; j < 243; j++) {
			off = j*lsz + 1;

			if (write(ofd, (char *)&pic[off], rsz) != rsz) {
				perror("write");
				close(ofd);
				return(-1);
			}

		}

		close(ofd);
	}

	return(0);
}
