#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#define global static
#define internal static

typedef int64_t int64;
typedef int bool32;
typedef unsigned char uchar;

struct platform_GameInput
{
  float secondsElapsed;
  float dtForFrame;
  bool32 leftIsDown;
  bool32 upIsDown;
  bool32 rightIsDown;
  bool32 downIsDown;
  bool32 spaceIsDown;
  bool32 rIsDown;
  bool32 sIsDown;
};

struct platform_RenderBuffer
{
  int width;
  int height;
  void *memory;
};

void platform_debugString(char *buffer);
void *platform_malloc(int size);
void platform_free(void *);
void platform_assert(bool32 ok);
//random number between 0.f and 1.f;
float platform_rand();

struct platform_Bitmap
{
  int imageWidth;
  int imageHeight;
  int bpp;
  char *data;
  int getPixel32(int x, int y);
};
platform_Bitmap *platform_loadBitmap(char *filename);
void platform_freeBitmap(platform_Bitmap *bitmap);

template <class T>
T platform_rand(T low, T high)
{
  if (high < low)
  {
    T temp = high;
    high = low;
    low = temp;
  }

  T diff = high - low;
  
  float result = platform_rand() * diff + low;
  return result;
}

//game layer should implement this
void updateGame(platform_GameInput *gameInput);
void renderGame(platform_RenderBuffer *renderBuffer);
void initializeGame();
void freeGame();
#endif