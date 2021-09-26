#include "zzxoto_bitmap_font.h"
#include "zzxoto_renderer.h"

// struct BitmapFont
// {
//   BitmapFont(char *fontFilename);

//   void renderText(platform_RenderBuffer *renderBuffer, int x, int y, char *text);

// private:
//   Rect *characters;
//   int charactersLength;
//   platform_Bitmap *bitmap;
//   int spaceWidth;
//   int newLineHeight;
// };

BitmapFont::BitmapFont(char *fontFilename, int cellWidth, int cellHeight, int bgColor, int lineGap)
{
  this->bitmap = platform_loadBitmap("assets/font.bmp");
  platform_assert(this->bitmap != NULL);

  platform_Bitmap *bmp = this->bitmap;

  platform_assert(bmp->imageWidth % cellWidth == 0);
  platform_assert(bmp->imageHeight % cellHeight == 0);

  int cellColumns = bmp->imageWidth / cellWidth;
  int cellRows = bmp->imageHeight / cellHeight;

  this->characterInfoLength = cellColumns * cellRows;
  this->characterInfo = (CharacterInfo *) platform_malloc(sizeof(CharacterInfo) * this->characterInfoLength);

  //top of the tallest character
  //0 is the tallest possible value
  int topMost = cellHeight;

  //bottom of the character extending farthest below baseline
  //cellHeight - 1 is the bottomest possible value
  int bottomMost = 0;

  for (int row = 0; row < cellRows; row++)
  {
    for (int column  = 0; column < cellColumns; column++)
    {
      int characterI = row * cellColumns + column;

      int top, left, right, bottom;
      bool32 isEmpty = true;

      int cellLeft = cellWidth * column;
      int cellTop = cellHeight * row;
      
      //find non background leftmost pixel of this character
      for (int pCol = 0; pCol < cellWidth; pCol++)
      {
        for (int pRow = 0; pRow < cellHeight; pRow++)
        {
          int y = cellTop + pRow;
          int x = cellLeft + pCol;
          if (bmp->getPixel32(x, y) != bgColor)
          {
            isEmpty = false;
            left = pCol;
            goto GOTO_1;
          }
        }
      }
      GOTO_1:;

      if (isEmpty)
      {
        goto GOTO_4;
      }

      //find non background topmost pixel of this character
      for (int pRow = 0; pRow < cellHeight; pRow++)
      {
        for (int pCol = 0; pCol < cellWidth; pCol++)
        {
          int y = cellTop + pRow;
          int x = cellLeft + pCol;
          if (bmp->getPixel32(x, y) != bgColor)
          {
            top = pRow;
            goto GOTO_2;
          }
        }
      }
      GOTO_2:;

      //find non background rightmost pixel of this character
      for (int pCol = cellWidth - 1; pCol >= 0; pCol--)
      {
        for (int pRow = 0; pRow < cellHeight; pRow++)
        {
          int y = cellTop + pRow;
          int x = cellLeft + pCol;
          if (bmp->getPixel32(x, y) != bgColor)
          {
            right = pCol;
            goto GOTO_3;
          }
        }
      }
      GOTO_3:;

      //find non background bottomMost pixel of this character
      for (int pRow = cellHeight - 1; pRow >= 0; pRow--)
      {
        for (int pCol = 0; pCol < cellWidth; pCol++)
        {
          int y = cellTop + pRow;
          int x = cellLeft + pCol;
          if (bmp->getPixel32(x, y) != bgColor)
          {
            bottom = pRow;
            goto GOTO_4;
          }
        }
      }
      GOTO_4:;

      CharacterInfo *cInfo = &this->characterInfo[characterI];

      if (isEmpty)
      {
        cInfo->isEmpty = true;
      }
      else
      {
        Rect bbox;
        bbox.left = left;
        bbox.top = top;
        bbox.width = right - left;
        bbox.height = bottom - top;
        cInfo->bbox = bbox;

        if (top < topMost)
        {
          topMost = top;
        }
        if (bottom > bottomMost)
        {
          bottomMost = bottom;
        }
        
        cInfo->cellLeft = cellLeft;
        cInfo->cellTop = cellTop;
        cInfo->isEmpty = false;
      }
    }
  }

  for (int row = 0; row < cellRows; row++)
  {
    for (int column  = 0; column < cellColumns; column++)
    {
      int ci = row * cellColumns + column;
      CharacterInfo *cInfo = &this->characterInfo[ci];
      cInfo->topBearing = cInfo->bbox.top - topMost;
      int a = cInfo->bbox.top;
    }
  }

  this->lineHeight = bottomMost - topMost + lineGap;
  this->cellWidth = cellWidth;
  this->cellHeight = cellHeight;
  this->bgColor = bgColor;
}

void BitmapFont::kill()
{
  platform_freeBitmap(this->bitmap);
  platform_free(this->characterInfo);
}

//assumes x > 0 && y > 0
void BitmapFont::renderText(platform_RenderBuffer *renderBuffer, int x, int y, int color, const char *text)
{
  int y0 = y;
  int x0 = x;
  int spaceWidth = cellWidth / 2;
  int characterSpacingWidth = spaceWidth / 3;

  const char *c = text;
  while (*c != '\0')
  {
    if (*c == '\n')
    {
      y0 += this->lineHeight;
      x0 = x;
    }
    if (*c == ' ')
    {
      x0 += spaceWidth;
    }
    else if (*c < this->characterInfoLength)
    {
      CharacterInfo *cInfo = &this->characterInfo[(uchar) *c];

      if (!cInfo->isEmpty)
      {
        int cx0 = cInfo->cellLeft + cInfo->bbox.left;
        int cx1 = cx0 + cInfo->bbox.width;
        int cy0 = cInfo->cellTop + cInfo->bbox.top;
        int cy1 = cy0 + cInfo->bbox.height;

        //y_ and x_ are index into render buffer
        //while cx and cy are index into bitmap
        for (int cy = cy0, y_ = cInfo->topBearing + y0; cy <= cy1; cy++, y_++)
        {
          if (y_ >= renderBuffer->height)
          {
            break;
          }
          for (int cx = cx0, x_ = x0; cx <= cx1; cx++, x_++)
          {
            if (x_ >= renderBuffer->width)
            {
              break;
            }
            if (this->bitmap->getPixel32(cx, cy) != this->bgColor)
            {
              setPixel_xy(renderBuffer, x_, y_, color);
            }
          }
        }

        x0 += cInfo->bbox.width + characterSpacingWidth;
      }
    }

    c++;
  }
}