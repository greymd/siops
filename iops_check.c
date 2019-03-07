#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
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
  if (argc != 4 && argc != 5) {
    fprintf(stderr, "usage %s <filename> <block size[KiB]> <r/w> [<# of loops>]\n", progname);
    exit(EXIT_FAILURE);
  }

  int block_size = atoi(argv[2]) * 1024;
  if (block_size == 0) {
    fprintf(stderr, "block size should be > 0: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  printf("block size (KiB):%d\n", block_size / 1024);

  int write_flag;
  if (!strcmp(argv[3], "r")) {
    write_flag = false;
  } else if (!strcmp(argv[3], "w")) {
    write_flag = true;
  } else {
    fprintf(stderr, "r/w should be 'r' or 'w': %s\n", argv[3]);
    exit(EXIT_FAILURE);
  }


  int fd;
  if (write_flag)
    fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC|O_DIRECT, 0666);
  if (!write_flag)
    fd = open(argv[1], O_RDWR|O_DIRECT, 0666);
  if (fd == -1) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
  printf("filename:%s\n", argv[1]);


  int nloop;
  if (argc == 4) {
    nloop  = FILE_SIZE / block_size;
    printf("file size (MiB):%d\n", FILE_SIZE / 1024 / 1024);
  } else if (argc == 5) {
    nloop = atoi(argv[4]);
  }
  if (nloop == 0) {
    fprintf(stderr, "block size should be > 0: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
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
    ssize_t ret;
    if (write_flag) {
      ret = write(fd, buf, strlen(buf));
      if (ret < 0) {
        err(EXIT_FAILURE, "write() failed");
      }
    } else {
      ret = read(fd, buf, strlen(buf));
      if (ret < 0) {
        err(EXIT_FAILURE, "read() failed");
      }
    }
  }
  gettimeofday(&after, NULL);

  double sec = diff_sec(before, after);
  printf("time: %f\n", sec);
  printf("iops: %f\n", nloop/sec);
  printf("MiB/s: %f\n", nloop * block_size / 1024 / 1024 / sec);

  if (close(fd) == -1) {
    err(EXIT_FAILURE, "close() failed");
  }
  return 0;
}
