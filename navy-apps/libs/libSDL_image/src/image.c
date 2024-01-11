#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  /* printf("%s\n", filename); */
   FILE *f_img = fopen(filename, "r");
  /* printf("%p\n", f_img); */
  fseek(f_img, 0, SEEK_END);
  int size = ftell(f_img);
  fseek(f_img, 0, SEEK_SET);

  char *buf = (char *)malloc(size * sizeof(char));
  fread(buf, size, 1, f_img);

  SDL_Surface *surface = STBIMG_LoadFromMemory(buf, size);
  /* int fd = open(filename, O_RDONLY); */
  /* int size = seek(fd, 0, SEEK_END); */
  /* printf("%d\n", size); */
  /* printf("%p\n", surface); */
  fclose(f_img);
  free(buf);

  return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
