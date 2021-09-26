#include "zzxoto_game.h"
#include "zzxoto_math.h"
#include "zzxoto_platform.h"
#include "zzxoto_constants.h"
#include "zzxoto_bitmap_font.h"

global GameState g_gameState;
global Mat3 g_bottomLeftToTopLeftTransform;
global BitmapFont *g_bitmapFont;

void initializeGame()
{
  g_bottomLeftToTopLeftTransform = scaleY(-1.f) * translate({0.f, -(GAME_HEIGHT - 1)});
  g_bitmapFont = (BitmapFont *) platform_malloc(sizeof(BitmapFont));
  *g_bitmapFont = BitmapFont("assets/font.bmp", 32, 32, 0x000000, 2);

  g_gameState.gameStateType = GameState_Type::welcomeScreen;

  g_gameState.gameState_welcomeScreen = (GameState_WelcomeScreen *) platform_malloc(sizeof(GameState_WelcomeScreen));
  g_gameState.gameState_playing       = (GameState_Playing *) platform_malloc(sizeof(GameState_Playing));
  g_gameState.gameState_end           = (GameState_End *) platform_malloc(sizeof(GameState_End));

  *g_gameState.gameState_welcomeScreen = GameState_WelcomeScreen(&g_bottomLeftToTopLeftTransform, g_bitmapFont);
  *g_gameState.gameState_playing = GameState_Playing(&g_bottomLeftToTopLeftTransform, g_bitmapFont);
  *g_gameState.gameState_end = GameState_End(&g_bottomLeftToTopLeftTransform, g_bitmapFont);
}

void renderGame(platform_RenderBuffer *renderBuffer)
{
  switch(g_gameState.gameStateType)
  {
    case GameState_Type::welcomeScreen:
    {
      g_gameState.gameState_welcomeScreen->renderGame(renderBuffer);
    }
    break;
    case GameState_Type::playing:
    {
      g_gameState.gameState_playing->renderGame(renderBuffer);
    }
    break;
    case GameState_Type::end:
    {
      g_gameState.gameState_end->renderGame(renderBuffer);
    }
    break;
  }
}

void updateGame(platform_GameInput *gameInput)
{
  switch(g_gameState.gameStateType)
  {
    case GameState_Type::welcomeScreen:
    {
      bool32 ok = g_gameState.gameState_welcomeScreen->updateGame(gameInput);
      if (!ok)
      {
        g_gameState.gameStateType = GameState_Type::playing;
      }
    }
    break;
    case GameState_Type::playing:
    {
      bool32 ok = g_gameState.gameState_playing->updateGame(gameInput);
      if (!ok)
      {
        g_gameState.gameStateType = GameState_Type::end;
      }
    }
    break;
    case GameState_Type::end:
    {
      bool32 ok = g_gameState.gameState_end->updateGame(gameInput);
      if (!ok)
      {
        freeGame();
        initializeGame();
      }
    }
    break;
  }
}

void freeGame()
{
  g_gameState.gameState_welcomeScreen->kill();
  g_gameState.gameState_playing->kill();
  g_gameState.gameState_end->kill();
  platform_free(g_gameState.gameState_welcomeScreen);
  platform_free(g_gameState.gameState_playing);
  platform_free(g_gameState.gameState_end);
  g_bitmapFont->kill();
  platform_free(g_bitmapFont);
}