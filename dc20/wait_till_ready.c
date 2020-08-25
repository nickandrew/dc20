int wait_till_ready( fd ) {
  char r;
  int n;

  /* Wait for NULL byte to signal completion */
  if ((n = read(fd, (char *)&r, 1)) != 1) {
    perror("read");
    return -1;
  }
  return 0;
}
