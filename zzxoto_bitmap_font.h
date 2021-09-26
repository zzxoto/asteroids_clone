#ifndef ZZXOTO_BITMAP_FONT_H
#define ZZXOTO_BITMAP_FONT_H
#include "zzxoto_platform.h"
#include "zzxoto_math.h"

struct CharacterInfo
{
  //bounding box relative to cell in bitmap
  Rect bbox;
  
  int topBearing;
  int cellLeft;
  int cellTop;
  bool32 isEmpty;
};

struct BitmapFont
{
  BitmapFont(char *fontFilename, int cellWidth, int cellHeight, int bgColor, int lineGap);
  void kill();

  void renderText(platform_RenderBuffer *renderBuffer, int x, int y, int color, const char *text);

private:
  CharacterInfo *characterInfo;
  int characterInfoLength;
  platform_Bitmap *bitmap;
  int lineHeight;
  int bgColor;

  int cellWidth;
  int cellHeight;
};

#endif