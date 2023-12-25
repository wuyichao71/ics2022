#include <fs.h>
/* wuyc */
#include <ramdisk.h>
#include <device.h>
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
  /* wuyc */
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  /* wuyc */
#include "files.h"
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  {"/dev/fb", 0, 0, invalid_read, fb_write}
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

#define FS_CMD(cmd) { \
  Finfo finfo = file_table[fd]; \
  int new_offset = finfo.open_offset + len; \
  if (new_offset > finfo.size) \
  { \
    new_offset = finfo.size; \
    len = finfo.size - finfo.open_offset; \
  } \
  if (len == 0) \
    return 0; \
  cmd; \
  file_table[fd].open_offset = new_offset; \
  return len; \
} while(0)

/* #define DO(events) { \ */
/*   if (file_table[fd].##events == NULL) \ */
/*   { \ */
/*     FS_CMD(ramdisk_##events(buf, finfo.disk_offset + finfo.open_offset, len)); \ */
/*   } \ */
/*   else \ */
/*     return file_table[fd].##events(buf, 0, len); \ */
/* } while(0) */

size_t fs_write(int fd, const void *buf, size_t len)
{
  /* DO(write); */
  if (file_table[fd].write == NULL)
  {
    FS_CMD(ramdisk_write(buf, finfo.disk_offset + finfo.open_offset, len));
  }
  else
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
}
size_t fs_read(int fd, void *buf, size_t len)
{
  /* DO(write); */
  /* if (finfo.open_offset >= finfo.size || new_offset > finfo.size) */
    /* panic("Out of file"); */
  if (file_table[fd].read == NULL)
  {
    FS_CMD(ramdisk_read(buf, finfo.disk_offset + finfo.open_offset, len));
  }
  else
  {
    return file_table[fd].read(buf, 0, len);
  }
}

char *get_filename(int fd)
{
  return file_table[fd].name;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  size_t new_offset;
  switch (whence)
  {
    case SEEK_SET:
      new_offset = offset;
      break;
    case SEEK_CUR:
      new_offset = file_table[fd].open_offset + offset;
      break;
    case SEEK_END:
      new_offset = file_table[fd].size + offset;
      break;
    default:
      panic("should not reach here");
  }

  if (new_offset <= file_table[fd].size)
  {
    file_table[fd].open_offset = new_offset;
    return new_offset;
  }
  return -1;
}

int fs_close(int fd)
{
  fs_lseek(fd, 0, SEEK_SET);
  return 0;
}
/* wuyc */

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int fd = fs_open("/dev/fb", 0, 0);
  file_table[fd].size = cfg.width * cfg.height * sizeof(uint32_t);
  fs_close(fd);
}
