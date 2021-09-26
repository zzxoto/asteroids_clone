#ifndef ZZXOTO_RENDERER_H
#define ZZXOTO_RENDERER_H
#include "zzxoto_platform.h"
#include "zzxoto_math.h"

const int LINE_WIDTH = 1;
const int DRAW_TYPE_TRIANGLE = 1;
const int DRAW_TYPE_POLYGON = 2;

struct Model
{
  V2 *vertices;
  int *colors;
  int *indices;  
  int indicesCount;
  Mat3 transform;
  int drawType;
};
void setPixel_pos(platform_RenderBuffer *renderBuffer, int pos, int color);
void setPixel_xy(platform_RenderBuffer *renderBuffer, int x, int y, int color);
void drawCircle(platform_RenderBuffer *renderBuffer, V2 center, float radius, int color);
void drawCircle_2(platform_RenderBuffer *renderBuffer, V2 center, float radius, int color);
void clearRenderBuffer(platform_RenderBuffer *renderBuffer, int color);
void copyPixels_destWrap(Rect srcRect, platform_RenderBuffer *src, Rect destRect, platform_RenderBuffer *dest);
void drawRectangle_fill(platform_RenderBuffer *renderBuffer, Rect rect, int color);
void drawLine(platform_RenderBuffer *renderBuffer, V2 p1, V2 p2, int lineWidth, int color);
void drawDot(platform_RenderBuffer *renderBuffer, V2 p, int lineWidth, int color);
void drawModel_outline(platform_RenderBuffer *renderBuffer, Model *model);
#endif