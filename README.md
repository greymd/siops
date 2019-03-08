# Simple IOPS checker

Check volume IOPS and throughput.

## Usage

```
$ gcc -o siops siops.c
```

```
usage ./siops <filename> <block size[KiB]> <r/w> [<# of loops>]
```

```
$ ./siops test.txt 64 w
block size (KiB):64
filename:test.txt
file size (MiB):100
number of system call:1600
time: 1.449568
iops: 1103.777125
MiB/s: 68.986070
```


## Purpose

There are various tool for volume benchmarking.
They are nice and well-created.

On the other hand, they are complicated and it is difficult for testers to customize.
What this tool does is just counting number of `write` or `read` calls.
It is very easy to customize because it is too simple.

Let's customize as you like.
For example, add any flags you want to try.

```
fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC|O_DIRECT, 0666);
```
