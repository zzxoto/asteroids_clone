#include "zzxoto_game.h"
#include "zzxoto_constants.h"

global char *g_title = R"FOO(A S T E.....


Press [R] to restart)FOO";


GameState_End::GameState_End(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont)
{
  this->bitmapFont = bitmapFont;
}

void GameState_End::kill()
{

}

void GameState_End::renderGame(platform_RenderBuffer *renderBuffer)
{
  clearRenderBuffer(renderBuffer, COLOR_BACKGROUND);
  this->bitmapFont->renderText(renderBuffer, 100, 150, COLOR_TEXT, g_title);
}

bool32 GameState_End::updateGame(platform_GameInput *gameInput)
{
  bool32 result = true;
  if (gameInput->rIsDown)
  {
    result = false;
  }
  
  return result;
}