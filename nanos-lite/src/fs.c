#include <fs.h>
/* wuyc */
#include <ramdisk.h>
/* wuyc */

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

/* wuyc */
int fs_open(const char *pathname, int flags, int mode)
{
  int file_n = sizeof(file_table) / sizeof(Finfo);
  int i;
  for (i = 0; i < file_n; i++)
  {
    /* printf("%d\n", file_table[i].open_offset); */
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      return i;
    }
  }
  assert(i == file_n);
  return -1;
}
/* wuyc */

size_t fs_read(int fd, void *buf, size_t len)
{
  Finfo finfo = file_table[fd];
  int new_offset = finfo.open_offset + len;
  /* if (finfo.open_offset >= finfo.size || new_offset > finfo.size) */
    /* panic("Out of file"); */
  if (new_offset > finfo.size)
  {
    new_offset = finfo.size;
    len = finfo.size - finfo.open_offset;
  }
  
  if (len == 0)
    return 0;

  ramdisk_read(buf, finfo.disk_offset + finfo.open_offset, len);
  file_table[fd].open_offset = new_offset;
  return len;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
