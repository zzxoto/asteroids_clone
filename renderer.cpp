#include "zzxoto_renderer.h"

inline
void setPixel_pos(platform_RenderBuffer *renderBuffer, int pos, int color)
{
  *((int *) renderBuffer->memory + pos) = color;
}

inline
void setPixel_xy(platform_RenderBuffer *renderBuffer, int x, int y, int color)
{
  x = wrap<int>(x, renderBuffer->width - 1);
  y = wrap<int>(y, renderBuffer->height - 1);
  int pos = x + renderBuffer->width * y;
  *((int *) renderBuffer->memory + pos) = color;
}

inline
internal void setPixel_xy(platform_RenderBuffer *renderBuffer, int x, int y, int width, int color)
{
  if (width > 1)
  {
    int k = width / 2;
    int x0 = x - k;
    int y0 = y - k;
    int x1 = x + k;
    int y1 = y + k;
    if (width % 2 == 0)
    {
      x1 -= 1;
      y1 -= 1;
    }

    for (int y_ = y0; y_ <= y1; y_++)
    {
      for (int x_ = x0; x_ <= x1; x_++)
      {
        setPixel_xy(renderBuffer, x_, y_, color);
      }
    }
  }
  else
  {
    setPixel_xy(renderBuffer, x, y, color);
  } 
}

void drawRectangle_fill(platform_RenderBuffer *renderBuffer, Rect rect, int color)
{
  for (int y = 0; y < rect.height; y++)
  {
    int yOffset = (y + rect.top) * renderBuffer->width;
    for (int x = 0; x < rect.width; x++)
    {
      int pos = x + yOffset + rect.left;
      setPixel_pos(renderBuffer, pos, color);
    }
  }
}

void copyPixels_destWrap(Rect srcRect, platform_RenderBuffer *src, Rect destRect, platform_RenderBuffer *dest)
{
  for (int y = 0; y < srcRect.height; y++)
  {
    int yOffset = (y + srcRect.top) * src->width;

    for (int x = 0; x < srcRect.width; x++)
    {
      int srcPosition = yOffset + x + srcRect.left;

      int destY = (destRect.top + y) % dest->height;
      int destX = (destRect.left + x) % dest->width;

      int destPosition = destY * dest->width + destX;
      int color = *((int *) src->memory + srcPosition);
      setPixel_pos(dest, destPosition, color);
    }
  }
}

internal void drawLine_octant1(platform_RenderBuffer *renderBuffer, int x0, int x1, int y0, int y1,
                                int deltaX, int deltaY, int xDirection, int lineWidth, int color)
{
  int x = x0;
  int y = y0;
  int D = 2 * deltaX - deltaY;
  int D1Increment = 2 * deltaX;
  int D2Increment = 2 * (deltaX - deltaY);

  setPixel_xy(renderBuffer, x, y, lineWidth, color);
  while (y != y1)
  {
    y += 1;
    if (D < 0)
    {
      D += D1Increment;
    }
    else
    {
      x += xDirection;
      D += D2Increment;
    }
    setPixel_xy(renderBuffer, x, y, lineWidth, color);
  }
}

internal void drawLine_octant0(platform_RenderBuffer *renderBuffer, int x0, int x1, int y0, int y1, 
                                int deltaX, int deltaY, int xDirection, int lineWidth, int color)
{
  int x = x0;
  int y = y0;
  int D = 2 * deltaY - deltaX;
  int D1Increment = 2 * deltaY;
  int D2Increment = 2 * (deltaY - deltaX);
  
  setPixel_xy(renderBuffer, x, y, lineWidth, color);
  while(x != x1)
  {
    x += xDirection;
    if (D < 0)
    {
      D += D1Increment;
    }
    else
    {
      y += 1;
      D += D2Increment;
    }
    setPixel_xy(renderBuffer, x, y, lineWidth, color);
  }
}

//References to bresenham algorithm:
//Algorithm explanation and derivation of first octant: https://www.ics.uci.edu/~gopi/CS112/web/handouts/OldFiles/Bresenham.pdf
//Algorithm adaptation to other octants: http://www.phatcode.net/res/224/files/html/ch35/35-03.html
void drawLine(platform_RenderBuffer *renderBuffer, V2 p1, V2 p2, int lineWidth, int color)
{
  platform_assert(lineWidth >= 1);
  //Limting line drawing cases to octant 0-3 by having delta_y always greater than 0
  if (p1.y > p2.y)
  {
    V2 temp = p1;
    p1 = p2;
    p2 = temp;   
  }

  int x0 = roundToInt(p1.x);
  int x1 = roundToInt(p2.x);
  int y0 = roundToInt(p1.y);
  int y1 = roundToInt(p2.y);

  int deltaX = x1 - x0;
  int deltaY = y1 - y0;

  if (deltaX > 0)
  {
    if (deltaX > deltaY)
    {
      drawLine_octant0(renderBuffer, x0, x1, y0, y1, deltaX, deltaY, 1, lineWidth, color);
    }
    else
    {
      drawLine_octant1(renderBuffer, x0, x1, y0, y1, deltaX, deltaY, 1, lineWidth, color);
    }
  }
  else
  {
    deltaX *= -1;
    if (deltaX > deltaY)
    {
      drawLine_octant0(renderBuffer, x0, x1, y0, y1, deltaX, deltaY, -1, lineWidth, color);
    }
    else
    {
      drawLine_octant1(renderBuffer, x0, x1, y0, y1, deltaX, deltaY, -1, lineWidth, color);
    }
  }
}

internal void drawCircle_symmetricPoints(platform_RenderBuffer *renderBuffer, int cx, int cy, int x, int y, int color)
{
  if (x == 0)
  {
    setPixel_xy(renderBuffer, cx, cy + y, color);
    setPixel_xy(renderBuffer, cx, cy - y, color);
    setPixel_xy(renderBuffer, cx + y, cy, color);
    setPixel_xy(renderBuffer, cx - y, cy, color);
  }
  else if (x == y)
  {
    setPixel_xy(renderBuffer, cx + x, cy + y, color);
    setPixel_xy(renderBuffer, cx - x, cy + y, color);
    setPixel_xy(renderBuffer, cx + x, cy - y, color);
    setPixel_xy(renderBuffer, cx - x, cy - y, color);
  }
  else
  {
    setPixel_xy(renderBuffer, cx + x, cy + y, color);
    setPixel_xy(renderBuffer, cx - x, cy + y, color);
    setPixel_xy(renderBuffer, cx + x, cy - y, color);
    setPixel_xy(renderBuffer, cx - x, cy - y, color);
    setPixel_xy(renderBuffer, cx + y, cy + x, color);
    setPixel_xy(renderBuffer, cx - y, cy + x, color);
    setPixel_xy(renderBuffer, cx + y, cy - x, color);
    setPixel_xy(renderBuffer, cx - y, cy - x, color);
  }
}

void drawCircle(platform_RenderBuffer *renderBuffer, V2 center, float radius, int color)
{
  int cx = roundToInt(center.x);
  int cy = roundToInt(center.y);
  int r = roundToInt(radius);

  int x = 0;
  int y = r;
  int p = (5 - r * 4) / 4;

  drawCircle_symmetricPoints(renderBuffer, cx, cy, x, y, color);
  
  while (x < y)
  {
    x++;
    if (p < 0)
    {
      p += 2 * x + 1;
    }
    else
    {
      y--;
      p += 2 * (x - y + 1);
    }
    drawCircle_symmetricPoints(renderBuffer, cx, cy, x, y, color);    
  }
}

void drawCircle_2(platform_RenderBuffer *renderBuffer, V2 center, float radius, int color)
{
  int cx = roundToInt(center.x);
  int cy = roundToInt(center.y);
  int r = roundToInt(radius);

  int x = 0;
  int y = r;
  int p = (5 - r * 4) / 4;

  drawCircle_symmetricPoints(renderBuffer, cx, cy, x, y, color);
  
  while (x < y)
  {
    x++;
    if (p < 0)
    {
      p += 2 * x + 1;
    }
    else
    {
      y--;
      p += 2 * (x - y) + 1;
    }
    drawCircle_symmetricPoints(renderBuffer, cx, cy, x, y, color);    
  }
}

void drawDot(platform_RenderBuffer *renderBuffer, V2 p, int width, int color)
{
  int x = roundToInt(p.x);
  int y = roundToInt(p.y);
  setPixel_xy(renderBuffer, x, y, width, color);
}

void drawModel_outline(platform_RenderBuffer *renderBuffer, Model *model)
{
  if (model->drawType == DRAW_TYPE_TRIANGLE)
  {
    platform_assert(model->indicesCount % 3 == 0);
    for (int i = 0; i < model->indicesCount; i += 3)
    {
      int i1 = model->indices[i];
      int i2 = model->indices[i + 1];
      int i3 = model->indices[i + 2];
      V2 v1 = model->transform * model->vertices[i1];
      V2 v2 = model->transform * model->vertices[i2];
      V2 v3 = model->transform * model->vertices[i3];

      drawLine(renderBuffer, v1, v2, LINE_WIDTH, model->colors[i1]);
      drawLine(renderBuffer, v2, v3, LINE_WIDTH, model->colors[i2]);
      drawLine(renderBuffer, v1, v3, LINE_WIDTH, model->colors[i3]);
    }
  }
  if (model->drawType == DRAW_TYPE_POLYGON)
  {
    platform_assert(model->indicesCount > 1);
    for (int i = 0; i < model->indicesCount - 1; i++)
    {
      int i1 = model->indices[i];
      int i2 = model->indices[i + 1];
      
      V2 v1 = model->transform * model->vertices[i1];
      V2 v2 = model->transform * model->vertices[i2];

      drawLine(renderBuffer, v1, v2, LINE_WIDTH, model->colors[i1]);
    }
  }
}

void clearRenderBuffer(platform_RenderBuffer *renderBuffer, int color)
{
  for (int y = 0; y < renderBuffer->height; y++)
  {
    int yOffset = renderBuffer->width * y;
    for (int x = 0; x < renderBuffer->width; x++)
    {
      int pos = yOffset + x;
      setPixel_pos(renderBuffer, pos, color);
    }
  }
}