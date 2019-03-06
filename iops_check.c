#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <err.h>

#define NLOOP 1000
#define NSECS_PER_SEC 1000000000UL

static char *progname;

static inline long diff_nsec(struct timespec before, struct timespec after) {
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                        - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

int main (int argc, char *argv[])
{
  progname = argv[0];
  if (argc != 3) {
    fprintf(stderr, "usage %s <filename> <block size[KB]>\n", progname);
    exit(EXIT_FAILURE);
  }

  int fd;
  fd = open(argv[1], O_CREAT|O_RDWR|O_SYNC|O_TRUNC|O_DIRECT, 0666);
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
  printf("block size:%d\n", block_size);

  char *buf;
  int e;
  e = posix_memalign((void **)&buf, 512, block_size + 1);
  if (e) {
    errno = e;
    err(EXIT_FAILURE, "posix_memalign() failed");
  }

  int i;
  for ( i = 0; i < block_size; i++) {
    buf[i] = 'a';
  }
  buf[block_size] = '\0';
  struct timespec before, after;

  clock_gettime(CLOCK_MONOTONIC, &before);
  for ( i = 0; i < NLOOP; i++) {
    write(fd, buf, strlen(buf));
  }
  clock_gettime(CLOCK_MONOTONIC, &after);
  printf("diff: %f\n", (double)diff_nsec(before, after));

  if (close(fd) == -1) {
    fprintf(stderr, "close() failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}
