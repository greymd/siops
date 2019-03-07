#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <err.h>

static char *progname;

static inline double diff_sec(struct timeval before, struct timeval after) {
  double sec = (double)(after.tv_sec - before.tv_sec);
  double nsec = (double)(after.tv_usec - before.tv_usec);
  return sec + nsec / 1000.0 / 1000.0;
}

int main (int argc, char *argv[])
{
  progname = argv[0];
  if (argc != 2 || argc != 3) {
    fprintf(stderr, "usage %s <filename> <# of loops>\n", progname);
    exit(EXIT_FAILURE);
  }
  int nloop = 1000;
  if (argc == 3) nloop = atoi(argv[2]);
  printf("filename:%s\n", argv[1]);
  printf("# of loops:%d\n", nloop);

  struct timeval before, after;
  gettimeofday(&before, NULL);
  int i, fd;
  for ( i = 0; i < nloop; i++) {
    fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC|O_DIRECT, 0666);
    if (fd == -1) {
      err(EXIT_FAILURE, "open() failed");
    }
    if (close(fd) == -1) {
      err(EXIT_FAILURE, "close() failed");
    }
  }
  gettimeofday(&after, NULL);

  double sec = diff_sec(before, after);
  printf("time: %f\n", sec);
  printf("call/s: %f\n", nloop * 2/sec);

  return 0;
}
