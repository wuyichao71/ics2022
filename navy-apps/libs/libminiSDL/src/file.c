#include <sdl-file.h>
/* wuyc */
/* #include <SDL.h> */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/* wuyc */

int64_t RWops_size(SDL_RWops *f)
{
  /* printf("In RWops_size\nSDL_size = %d\n", f->mem.size); */
  return f->mem.size;
}

int64_t RWops_seek(SDL_RWops *f, int64_t offset, int whence)
{
  return fseek(f->fp, offset, whence);
  /* printf("In RWops_seek\n"); */
  /* assert(0); */
  /* return 0; */
}

size_t RWops_read(SDL_RWops *f, void *buf, size_t size, size_t nmemb)
{
  return fread(buf, size, nmemb, f->fp);
  /* printf("In RWops_read\n"); */
  /* assert(0); */
  /* return 0; */
}

size_t RWops_write(SDL_RWops *f, const void *buf, size_t size, size_t nmemb)
{
  return fwrite(buf, size, nmemb, f->fp);
  /* printf("In RWops_write\n"); */
  /* assert(0); */
  /* return 0; */
}

int RWops_close(SDL_RWops *f)
{
  /* printf("In RWops_close\n"); */
  fclose(f->fp);
  free(f);
  /* assert(0); */
  return 0;
}

SDL_RWops* SDL_RWFromFile(const char *filename, const char *mode) {
  SDL_RWops *ops = (SDL_RWops *)malloc(sizeof(SDL_RWops));
  *ops = (SDL_RWops){
    .size = RWops_size, 
    .seek = RWops_seek, 
    .read = RWops_read, 
    .write = RWops_write,
    .close = RWops_close, 
    .type = RW_TYPE_FILE,
    .fp = fopen(filename, mode),
  };
  /* ops->fp = fopen(filename, mode); */
  fseek(ops->fp, 0, RW_SEEK_END);
  int64_t size = ftell(ops->fp);
  fseek(ops->fp, 0, RW_SEEK_SET);
  ops->mem.size = size;
  /* printf("size = %d\n", size); */
  /* return NULL; */
  return ops;
}

SDL_RWops* SDL_RWFromMem(void *mem, int size) {
  SDL_RWops *ops = (SDL_RWops *)malloc(sizeof(SDL_RWops));
  *ops = (SDL_RWops){
    .size = RWops_size, 
    .seek = RWops_seek, 
    .read = RWops_read, 
    .write = RWops_write,
    .close = RWops_close, 
    .type = RW_TYPE_MEM,
    .fp = fmemopen(mem, size, "r+"),
  };
  /* fseek(ops->fp, 0, RW_SEEK_END); */
  /* int64_t mem_size = ftell(ops->fp); */
  /* fseek(ops->fp, 0, RW_SEEK_SET); */
  ops->mem.size = size;
  ops->mem.base = mem;
  return ops;
  /* assert(0); */
  /* return NULL; */
}
