#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <err.h>

// use fixed sector size because its purpose is just to check iops
#define SECTOR_SIZE 512
#define FILE_SIZE 100 * 1024 * 1024

static char *progname;

static inline double diff_sec(struct timeval before, struct timeval after) {
  double sec = (double)(after.tv_sec - before.tv_sec);
  double nsec = (double)(after.tv_usec - before.tv_usec);
  return sec + nsec / 1000.0 / 1000.0;
}

int main (int argc, char *argv[])
{
  progname = argv[0];
  if (argc != 3) {
    fprintf(stderr, "usage %s <filename> <block size[KiB]>\n", progname);
    exit(EXIT_FAILURE);
  }

  int fd;
  fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC|O_DIRECT, 0666);
  if (fd == -1) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  printf("filename:%s\n", argv[1]);

  int block_size = atoi(argv[2]) * 1024;
  if (block_size == 0) {
    fprintf(stderr, "block size should be > 0: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  printf("block size (KiB):%d\n", block_size / 1024);
  printf("file size (MiB):%d\n", FILE_SIZE / 1024 / 1024);

  int nloop = FILE_SIZE / block_size;
  printf("number of system call:%d\n", nloop);

  char *buf;
  int e;
  e = posix_memalign((void **)&buf, SECTOR_SIZE, block_size + 1);
  if (e) {
    errno = e;
    err(EXIT_FAILURE, "posix_memalign() failed");
  }

  int i;
  for ( i = 0; i < block_size; i++) {
    buf[i] = 'a';
  }
  buf[block_size] = '\0';
  struct timeval before, after;

  gettimeofday(&before, NULL);
  for ( i = 0; i < nloop; i++) {
    write(fd, buf, strlen(buf));
  }
  gettimeofday(&after, NULL);

  double sec = diff_sec(before, after);
  printf("time: %f\n", sec);
  printf("iops: %f\n", nloop/sec);
  printf("MiB/s: %f\n", nloop * block_size / 1024 / 1024 / sec);

  if (close(fd) == -1) {
    fprintf(stderr, "close() failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}
