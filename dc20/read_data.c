#include <stdio.h>

read_data(fd, buf, sz)
int fd;
unsigned char *buf;
int sz;
{
	unsigned char ccsum;
	unsigned char rcsum;
	unsigned char c;
	int n;
	int r;
	int i;

	for (n = 0; n < sz && (r = read(fd, (char *)&buf[n], sz - n)) > 0; n += r)
		;

	if (r <= 0) {
		perror("read: r <= 0");
		return(-1);
	}

	if (n < sz || read(fd, &rcsum, 1) != 1) {
		perror("read");
		return(-1);
	}

	for (i = 0, ccsum = 0; i < n; i++)
		ccsum ^= buf[i];

	if (ccsum != rcsum) {
		fprintf(stderr, "Bad checksum (%02x != %02x)\n", rcsum, ccsum);
		return(-1);
	}

	c = 0xd2;

	if (write(fd, (char *)&c, 1) != 1) {
		perror("write");
		return(-1);
	}

	return(0);
}

end_of_data(fd)
int fd;
{
	char c;

	if (read(fd, &c, 1) != 1) {
		perror("read");
		return(-1);
	}

	if (c != 0) {
		fprintf(stderr, "Bad EOD from camera (%02x).\n", (unsigned)c);
		return(-1);
	}

	return(0);
}
