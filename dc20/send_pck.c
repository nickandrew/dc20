send_pck(fd, pck)
int fd;
unsigned char *pck;
{
	int n;
	unsigned char r;

	if (write(fd, (char *)pck, 8) != 8) {
		perror("write");
		return(-1);
	}

	if ((n = read(fd, (char *)&r, 1)) != 1) {
		perror("read");
		return(-1);
	}

	return((r == 0xd1) ? 0 : -1);
}
