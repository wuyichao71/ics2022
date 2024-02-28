#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
/* wuyc */
#include <SDL.h>
#include <stdio.h>
void CallbackHelper();
void TimerCallbackHelper();
extern uint32_t g_flags;
/* wuyc */

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  int sw = src->w, sh = src->h;
  int srx = 0, sry = 0, srw = sw, srh = sh;
  if (srcrect != NULL)
  {
    srx = srcrect->x;
    sry = srcrect->y;
    if (srx >= sw || sry >= sh)
      return;
    srw = srx + srcrect->w > sw ? sw - srx : srcrect->w;
    srh = sry + srcrect->h > sh ? sh - sry : srcrect->h;
  }
  /* printf("%d, %d\n", src->w, src->h); */
  /* printf("%d, %d\n", dst->w, dst->h); */

  int dw = dst->w, dh = dst->h;
  int drx = 0, dry = 0;
  if (dstrect != NULL)
  {
    drx = dstrect->x;
    dry = dstrect->y;
    if (drx >= dw || dry >= dh)
      return;
  }
  srw = drx + srw > dw ? dw - drx : srw;
  srh = dry + srh > dh ? dh - dry : srh;
  /* printf("%d, %d\n", srw, srh); */

  if (src->format->BitsPerPixel == 32)
  {
    int src_offset = (sw * sry + srx) * sizeof(uint32_t);
    int dst_offset = (dw * dry + drx) * sizeof(uint32_t);
    for (int i = 0; i < srh; i++)
    {
      memcpy(dst->pixels + dst_offset, src->pixels + src_offset, srw * sizeof(uint32_t));
      src_offset += sw * sizeof(uint32_t);
      dst_offset += dw * sizeof(uint32_t);
    }
  }
  else if (src->format->BitsPerPixel == 8)
  {
    int src_offset = (sw * sry + srx) * sizeof(uint8_t);
    int dst_offset = (dw * dry + drx) * sizeof(uint8_t);
    for (int i = 0; i < srh; i++)
    {
      memcpy(dst->pixels + dst_offset, src->pixels + src_offset, srw * sizeof(uint8_t));
      src_offset += sw * sizeof(uint8_t);
      dst_offset += dw * sizeof(uint8_t);
    }
  }
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
  /* if (!(dstrect->x < dst->w && dstrect->y < dst->h && */ 
        /* dstrect->x + dstrect->w < dst->w && dstrect->y + dstrect->h < dst->h)) */
    /* return -1; */
  /* void CallbackHelper(); */
  int dw = dst->w, dh = dst->h;
  int offset = 0;
  if (dstrect == NULL)
  {
    for (int j = 0; j < dh; j++)
    {
      for (int i = 0; i < dw; i++)
      {
        /* printf("%d\n", dst->pixels); */
        ((uint32_t *)dst->pixels)[offset+i] = color;
        /* offset++; */
      }
      offset += dw;
    }
  }
  else
  {
    int drx, dry, drw, drh;
    drx = dstrect->x;
    dry = dstrect->y;
    drw = drx + dstrect->w > dw ? dw - drx : dstrect->w;
    drh = dry + dstrect->h > dh ? dh - dry : dstrect->h;
    if (drx >= dw || dry >= dh)
      return;
    offset = (dry * dw + drx) * sizeof(uint32_t);
    for (int j = 0; j < drh; j++)
    {
      for (int i = 0; i < drw; i++)
      {
        /* printf("%d\n", dst->pixels); */
        ((uint32_t *)dst->pixels)[offset+i] = color;
        /* offset++; */
      }
      /* offset += dw - drw; */
      offset += dw;
    }
  }
  /* return 0; */
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {
  /* NDL_OpenCanvas(&s->w, &s->h); */
  CallbackHelper();
  if (g_flags & SDL_INIT_TIMER) TimerCallbackHelper();
  if (x == 0 && y == 0 && w == 0 && h == 0)
  {
    w = s->w; h = s->h;
    /* NDL_DrawRect((uint32_t *)s->pixels, 0, 0, s->w, s->h); */
    /* printf("I am here3\n"); */
  }
  uint32_t len = w * h;
  uint32_t *buf = (uint32_t *)malloc(len * sizeof(uint32_t));
  uint32_t offset = y * s->w + x;
  int i = 0;
    /* NDL_OpenCanvas(s->w, s->h); */
    /* printf("I am here3\n"); */
  for (int row = 0; row < h; row++)
  {
    for (int col = 0; col < w; col++)
    {
      if (s->format->BitsPerPixel == 32)
        buf[i++] = ((uint32_t *)s->pixels)[offset + col];
      else if (s->format->BitsPerPixel == 8)
      {
        SDL_Color color = s->format->palette->colors[((uint8_t *)s->pixels)[offset + col]];
        buf[i++] = (uint32_t)(color.a << 24 | color.r << 16 | color.g << 8 | color.b);
        /* buf[i] = ((uint8_t *)s->format->palette->colors)[((uint8_t *)s->pixels)[i]]; */
      }
    }
    offset += s->w;
  }
  /* printf("x = %d, y = %d, w = %d, h = %d\n", x, y, w, h); */
  NDL_DrawRect(buf, x, y, w, h);
  free(buf);
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
}
