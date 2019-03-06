#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 128
#define NLOOP 1000
#define NSECS_PER_SEC 1000000000UL

static inline long diff_nsec(struct timespec before, struct timespec after) {
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                        - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

int main (int argc, char *argv[])
{
  int fd;
  char buf[BUF_SIZE + 1];
  // fd = open(argv[1], O_CREAT|O_WRONLY|O_TRUNC|O_DIRECT, 0755);
  // fd = open(argv[1], O_CREAT|O_WRONLY|O_DIRECT, 0755);
  fd = open(argv[1], O_RDWR|O_DIRECT, 0755);
  if (fd == -1) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  int i;
  for ( i = 0; i < BUF_SIZE; i++) {
    buf[i] = 'a';
  }
  buf[BUF_SIZE] = '\0';
  printf("buf:%s\n",buf);
  struct timespec before, after;

  // printf("strlen(buf) = %d\n", strlen(buf));
  clock_gettime(CLOCK_MONOTONIC, &before);
  int wnum;
  for ( i = 0; i < NLOOP; i++) {
    wnum = write(fd, buf, strlen(buf));
    printf("wnum: %d\n", wnum);
  }
  clock_gettime(CLOCK_MONOTONIC, &after);
  printf("diff: %f\n", (double)diff_nsec(before, after));

  if (close(fd) == -1) {
    fprintf(stderr, "close() failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}
