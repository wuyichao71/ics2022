#include <common.h>
#include "syscall.h"

/* wuyc */
#include <proc.h>
#include <loader.h>
#include <fs.h>
#include <sys/time.h>
void switch_boot_pcb();
/* #define __STRACE__ */
#ifdef __STRACE__
# define SYS_format(format) format "         = %d"
# define STRACE(SYS_type, format, ...) printf(#SYS_type format "\n", ## __VA_ARGS__)
#else
# define SYS_format(format) 
# define STRACE(SYS_type, format, ...) 
#endif

void sys_exit(int code);
uintptr_t sys_yield();
int sys_open(const char *path, int flags, int mode);
size_t sys_read(int fd, void *buf, size_t len);
size_t sys_write(int fd, const void *buf, size_t count);
size_t sys_lseek(int fd, size_t offset, int whence);
int sys_close(int fd);
int sys_brk(intptr_t addr);
int sys_execve(const char *fname, char *const argv[], char *const envp[]);
int sys_gettimeofday(struct timeval *tv, struct timezone *tz);
int mm_brk(uintptr_t brk);

/* wuyc */
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  /* wuyc */
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  /* wuyc */

  switch (a[0]) {
    /* wuyc */
    case SYS_exit: 
      STRACE(sys_exit, "(%d)", a[1]); 
      sys_exit(a[1]); 
      break;
    /* case SYS_exit: c->GPRx=0; break; */
    case SYS_yield: 
      c->GPRx = sys_yield(); 
      STRACE(sys_yield, SYS_format("()"), c->GPRx); 
      break;
    case SYS_open:
      c->GPRx = sys_open((const char *)a[1], a[2], a[3]);
      STRACE(sys_open, SYS_format("(\"%s\", %d, %d)"), a[1], a[2], a[3], c->GPRx);
      break;
    case SYS_read:
      c->GPRx = sys_read(a[1], (char *)a[2], a[3]);
      STRACE(sys_read, SYS_format("(\"%s\", 0x%08x, %d)"), get_filename(a[1]), a[2], a[3], c->GPRx);
      break; 
    case SYS_write: 
      c->GPRx = sys_write(a[1], (const void *)a[2], a[3]); 
      STRACE(sys_write, SYS_format("(\"%s\", 0x%08x, %d)"), get_filename(a[1]), a[2], a[3], c->GPRx); 
      break;
    case SYS_lseek:
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      STRACE(sys_lseek, SYS_format("(\"%s\", %d, %d)"), get_filename(a[1]), a[2], a[3], c->GPRx); 
      break;
    case SYS_close:
      c->GPRx = sys_close(a[1]);
      STRACE(sys_close, SYS_format("(\"%s\")"), get_filename(a[1]), c->GPRx);
      break;
    case SYS_brk: 
      c->GPRx = sys_brk(a[1]); 
      STRACE(sys_brk, SYS_format("(0x%08x)"), a[1], c->GPRx);
      break;
    case SYS_execve:
      c->GPRx = sys_execve((const char *)a[1], (char *const*)a[2], (char *const*)a[3]);
      STRACE(sys_execve, SYS_format("(\"%s\", 0x%08x, 0x%08x)"), get_filename(a[1]), a[2], a[3], c->GPRx);
    case SYS_gettimeofday:
      c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
      STRACE(sys_gettimeofday, SYS_format("(0x%08x, 0x%08x)"), a[1], a[2]);
      break;
    /* wuyc */
    default: 
      panic("Unhandled syscall ID = %d", a[0]);
  }
  /* printf("HHH\n"); */
}

uintptr_t sys_yield()
{
  yield();
  return (uintptr_t)0;
}

/* wuyc */
void sys_exit(int code)
{
  /* naive_uload(NULL, "/bin/nterm"); */
  /* char *argv[2] = {NULL}; */
  /* char *envp[2] = {NULL}; */
  /* argv[0] = "/bin/nterm"; */
  /* envp[0] = "PATH=/bin"; */
  /* context_uload(current, "/bin/nterm", argv, envp); */
  /* switch_boot_pcb(); */
  /* yield(); */
  halt(code);
}

int sys_open(const char *path, int flags, int mode)
{
  return fs_open(path, flags, mode);
}

size_t sys_write(int fd, const void *buf, size_t count)
{
  return fs_write(fd, buf, count);
}

size_t sys_read(int fd, void *buf, size_t len)
{
  return fs_read(fd, buf, len);
}

size_t sys_lseek(int fd, size_t offset, int whence)
{
  return fs_lseek(fd, offset, whence);
}

int sys_close(int fd)
{
  return fs_close(fd);
}

int sys_brk(intptr_t addr)
{
  return mm_brk(addr);
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us % 1000000;
  return 0;
}

int sys_execve(const char *fname, char *const argv[], char *const envp[])
{
  int fd = fs_open(fname, 0, 0);
  if (fd == -1)
    return -2;
  else
    fs_close(fd);
  context_uload(current, fname, argv, envp);
  switch_boot_pcb();
  yield();

  /* naive_uload(NULL, fname); */
  return 0;
}

/* wuyc */
