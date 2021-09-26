#ifndef ZZXOTO_GAME_H
#define ZZXOTO_GAME_H

#include "zzxoto_platform.h"
#include "zzxoto_renderer.h"
#include "zzxoto_player.h"
#include "zzxoto_asteroid.h"
#include "zzxoto_bitmap_font.h"

struct GameState_Playing;
struct GameState_WelcomeScreen;
struct GameState_End;
struct GameState;

enum class GameState_Type
{
  welcomeScreen=1,
  playing,
  end
};

struct GameState
{
  GameState_Type gameStateType;

  GameState_Playing *gameState_playing;
  GameState_WelcomeScreen *gameState_welcomeScreen;
  GameState_End *gameState_end;
};

struct GameState_Playing
{
  int playerHealth;
  Model *healthModels;
  Model playerModel;

  BitmapFont *bitmapFont;  

  Player *player;
  float playerRespawnsAt;

  int score;

  Asteroid *asteroids;
  int asteroidsListSize;
  Mat3 *bottomLeftToTopLeftTransform;  

  GameState_Playing(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont);
  void renderGame(platform_RenderBuffer *renderBuffer);
  bool32 updateGame(platform_GameInput *gameInput);
  void kill();

private:
  Asteroid createAsteroid(int asteroidType);
  void collision(platform_GameInput *gameInput, Asteroid *asteroid);
  void collision(platform_GameInput *gameInput, Bullet *bullet);
  void collision(platform_GameInput *gameInput, Player *player);
  bool32 didCollide(Asteroid *asteroid, Bullet *bullet);
  bool32 didCollide(Asteroid *asteroid, Player *player);
  bool32 didCollide(V2 p1, float r1, V2 p2, float r2);
};

struct GameState_WelcomeScreen
{
  Asteroid *asteroids;
  int asteroidsListSize;
  Mat3 *bottomLeftToTopLeftTransform;  
  BitmapFont *bitmapFont;

  GameState_WelcomeScreen(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont);
  void renderGame(platform_RenderBuffer *renderBuffer);
  bool32 updateGame(platform_GameInput *gameInput);
  void kill();
private:
  Asteroid createAsteroid(int asteroidType);
};

struct GameState_End
{
  BitmapFont *bitmapFont;

  GameState_End(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont);
  void renderGame(platform_RenderBuffer *renderBuffer);
  bool32 updateGame(platform_GameInput *gameInput);
  void kill();
};
#endif