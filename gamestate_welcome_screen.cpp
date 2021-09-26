#include "zzxoto_game.h"
#include "zzxoto_constants.h"
#include "zzxoto_asteroid.h"
#include "zzxoto_bitmap_font.h"

global char *g_title = R"FOO(A S T E R O I D


Press [S] to start)FOO";

GameState_WelcomeScreen::GameState_WelcomeScreen(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont)
{
  this->bitmapFont = bitmapFont;  
  this->bottomLeftToTopLeftTransform = bottomLeftToTopLeftTransform;
  
  this->asteroidsListSize = WELCOMESCREEN_ASTEROID_LARGE_COUNT
    + WELCOMESCREEN_ASTEROID_MEDIUM_COUNT
    + WELCOMESCREEN_ASTEROID_SMALL_COUNT;

  this->asteroids = (Asteroid *) platform_malloc(sizeof(Asteroid) * this->asteroidsListSize);

  int count = 0;
  for (int i = 0; i < WELCOMESCREEN_ASTEROID_LARGE_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_LARGE);
  }
  for (int i = 0; i < WELCOMESCREEN_ASTEROID_MEDIUM_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_MEDIUM);
  }
  for (int i = 0; i < WELCOMESCREEN_ASTEROID_SMALL_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_SMALL);
  }
}

Asteroid GameState_WelcomeScreen::createAsteroid(int asteroidType)
{
  V2 translate = { platform_rand(0.f, (float) GAME_WIDTH - 1), platform_rand(0.f, (float) GAME_HEIGHT - 1) };
  return Asteroid(this->bottomLeftToTopLeftTransform, asteroidType, translate);
}

void GameState_WelcomeScreen::kill()
{
  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      ast->kill();
    }   
  }
  platform_free(this->asteroids);
}

void GameState_WelcomeScreen::renderGame(platform_RenderBuffer *renderBuffer)
{
  clearRenderBuffer(renderBuffer, COLOR_BACKGROUND);

  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      ast->render(renderBuffer);
    }
  }

  this->bitmapFont->renderText(renderBuffer, 100, 150, COLOR_TEXT, g_title);
}

bool32 GameState_WelcomeScreen::updateGame(platform_GameInput *gameInput)
{
  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      ast->update(gameInput);
    }
  }

  bool32 result = true;
  if (gameInput->sIsDown)
  {
    result = false;
  }
  
  return result;
}