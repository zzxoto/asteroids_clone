#include "zzxoto_game.h"
#include "zzxoto_constants.h"
#include "stdio.h"

global const float SCORE_PADDING_TOP = 10.f;
global const float SCORE_PADDING_RIGHT = 150.f;
global const float HEALTH_MODEL_PADDING_TOP   = SCORE_PADDING_TOP + 45.f;
global const float HEALTH_MODEL_PADDING_RIGHT = 100.f;

GameState_Playing::GameState_Playing(Mat3 *bottomLeftToTopLeftTransform, BitmapFont *bitmapFont)
{
  this->bottomLeftToTopLeftTransform = bottomLeftToTopLeftTransform;

  this->bitmapFont = bitmapFont;
 
  this->playerRespawnsAt = -1.f;
  this->score = 0.f;

  this->asteroidsListSize = (PLAYSCREEN_ASTEROID_LARGE_COUNT +  PLAYSCREEN_ASTEROID_MEDIUM_COUNT + PLAYSCREEN_ASTEROID_SMALL_COUNT) * 9;
  this->asteroids = (Asteroid *) platform_malloc(sizeof(Asteroid) * this->asteroidsListSize);

  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    this->asteroids[i].isKilled = true;
  }
  int count = 0;
  for (int i = 0; i < PLAYSCREEN_ASTEROID_LARGE_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_LARGE);
  }
  for (int i = 0; i < PLAYSCREEN_ASTEROID_MEDIUM_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_MEDIUM);
  }
  for (int i = 0; i < PLAYSCREEN_ASTEROID_SMALL_COUNT; i++)
  {
    this->asteroids[count++] = this->createAsteroid(ASTEROID_TYPE_SMALL);
  }

  {
    Model m       = {};
    m.vertices    = (V2 *) platform_malloc(sizeof(V2) * 3);
    m.indices     = (int *) platform_malloc(sizeof(V2) * 3);
    m.colors      = (int *) platform_malloc(sizeof(V2) * 3);
    m.vertices[0] = {0.f, .5f };
    m.vertices[1] = {-.25f,-.5f };
    m.vertices[2] = {.25f,-.5f };
    m.indices[0]  = 0;
    m.indices[1]  = 1;
    m.indices[2]  = 2;
    m.colors[0]   = COLOR_PLAYER;
    m.colors[2]   = COLOR_PLAYER;
    m.colors[1]   = COLOR_PLAYER;
    m.drawType    = DRAW_TYPE_TRIANGLE;
    m.transform   = identity();
    m.indicesCount = 3;
    this->playerModel = m;
  }

  this->player = (Player *) platform_malloc(sizeof(Player));
  *this->player = Player(bottomLeftToTopLeftTransform, this->playerModel);

  this->playerHealth = PLAYER_MAX_HEALTH;

  this->healthModels = (Model *) platform_malloc(sizeof(Model) * PLAYER_MAX_HEALTH);

  V2 healthModelTranslate = { GAME_WIDTH - 1 - HEALTH_MODEL_PADDING_RIGHT, GAME_HEIGHT - 1 - HEALTH_MODEL_PADDING_TOP };
  for (int i = PLAYER_MAX_HEALTH - 1; i >= 0; i--)
  {
    this->healthModels[i] = this->playerModel;
    this->healthModels[i].transform = *bottomLeftToTopLeftTransform * scaleRotateTranslate(PLAYER_HEALTH_MODEL_SCALE, 0.f, healthModelTranslate);
    healthModelTranslate.x -= PLAYER_HEALTH_MODEL_SCALE + 5.f;
  }
}

void GameState_Playing::kill()
{
  if (!this->player->isKilled)
  {
    this->player->kill();
  }  
  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    if (!this->asteroids[i].isKilled)
    {
      this->asteroids[i].kill();
    }
  }
  platform_free(this->player);
  platform_free(this->asteroids);
  platform_free(this->healthModels);
  platform_free(this->playerModel.vertices);
  platform_free(this->playerModel.colors);
  platform_free(this->playerModel.indices);
}

bool32 GameState_Playing::didCollide(V2 p1, float r1, V2 p2, float r2)
{
  float a = square(r1 + r2);
  float b = distanceSquared(p1, p2);

  bool32 result = b <= a;
  return result;
}

bool32 GameState_Playing::didCollide(Asteroid *asteroid, Bullet *bullet)
{
  bool32 result = didCollide(asteroid->translate, asteroid->collisionRadius, bullet->translate, bullet->collisionRadius);
  return result;
}

bool32 GameState_Playing::didCollide(Asteroid *asteroid, Player *player)
{
  bool32 result = didCollide(asteroid->translate, asteroid->collisionRadius, player->translate, player->collisionRadius);
  return result;
}

void GameState_Playing::collision(platform_GameInput *gameInput, Player *player)
{
  this->playerHealth--;
  this->player->kill();
  this->playerRespawnsAt = gameInput->secondsElapsed + PLAYER_RESPAWN_SECONDS;
}

void GameState_Playing::collision(platform_GameInput *gameInput, Bullet *bullet)
{
  bullet->kill();
}

void GameState_Playing::collision(platform_GameInput *gameInput, Asteroid *asteroid)
{
  asteroid->kill();

  int astType = asteroid->asteroidType;
  this->score += asteroid->score;

  if (astType != ASTEROID_TYPE_SMALL)
  {
    int asteroidsToCreate = 3;
    int asteroidTypeToCreate = ASTEROID_TYPE_MEDIUM;
    if (astType == ASTEROID_TYPE_MEDIUM)
    {
      asteroidTypeToCreate = ASTEROID_TYPE_SMALL;
    }
    for (int i = 0; i < this->asteroidsListSize && asteroidsToCreate > 0; i++)
    {
      Asteroid *ast = &this->asteroids[i];
      if (ast->isKilled)
      {
        *ast = Asteroid(this->bottomLeftToTopLeftTransform, asteroidTypeToCreate, asteroid->translate);        
        asteroidsToCreate--;
      }
    }
  }
}

Asteroid GameState_Playing::createAsteroid(int asteroidType)
{
  V2 translate = { platform_rand<float>(0, GAME_WIDTH - 1.f), platform_rand<float>(GAME_HEIGHT / 3.f, GAME_HEIGHT / 2.f) };
  return Asteroid(this->bottomLeftToTopLeftTransform, asteroidType, translate);
}

bool32 GameState_Playing::updateGame(platform_GameInput *gameInput)
{  


  if (!this->player->isKilled)
  {
    //when player is animating on the screen
    //
    this->player->update(gameInput);

    //check bullet and asteroid collision
    for (int i = 0; i < this->player->bulletListSize; i++)
    {
      Bullet *bullet = &this->player->bullets[i];

      if (!bullet->isKilled)
      {
        for (int j = 0; j < this->asteroidsListSize; j++)
        {
          Asteroid *ast = &this->asteroids[j];
          if (!ast->isKilled)
          {
            if (didCollide(ast, bullet))
            {
              collision(gameInput, ast);
              collision(gameInput, bullet);
              goto GOTO_NESTED_FOR_EXIT;
            }
          }
        }
      }
    }
    GOTO_NESTED_FOR_EXIT:;

    //check player and asteroid collision
    for (int j = 0; j < this->asteroidsListSize; j++)
    {
      Asteroid *ast = &this->asteroids[j];

      if (!ast->isKilled)
      {
        if (didCollide(ast, this->player))
        {
          collision(gameInput, ast);
          collision(gameInput, this->player);
          break;
        }
      }
    }
  }
  else if (this->playerHealth > 0 && gameInput->secondsElapsed > this->playerRespawnsAt)
  {
    //when its time to respawn player
    //
    *this->player = Player(this->bottomLeftToTopLeftTransform, this->playerModel);
  }
  
  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      ast->update(gameInput);
    }
  }

  bool32 result = true;
  if (this->playerHealth <= 0)
  {
    result = false;
  }

  return result;  
}

//#define DEBUG_COLLISION_CIRCLE
void GameState_Playing::renderGame(platform_RenderBuffer *renderBuffer)
{
  clearRenderBuffer(renderBuffer, COLOR_BACKGROUND);

  if (!this->player->isKilled)
  {
    this->player->render(renderBuffer);
  }

  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      ast->render(renderBuffer);
    }
  }

  for (int i = 0; i < this->playerHealth; i++)
  {
    drawModel_outline(renderBuffer, &this->healthModels[i]);
  }

  char buffer[8]; sprintf(buffer, "%d", this->score);
  this->bitmapFont->renderText(renderBuffer, GAME_WIDTH - 1 - SCORE_PADDING_RIGHT, SCORE_PADDING_TOP, COLOR_TEXT, buffer);
  

#ifdef DEBUG_COLLISION_CIRCLE 
  if (!this->player->isKilled)
  {
    drawCircle(renderBuffer, 
      *this->bottomLeftToTopLeftTransform * this->player->translate, 
      this->player->collisionRadius, 
      COLOR_COLLISION_CIRCLE);
  }

  for (int i = 0; i < this->asteroidsListSize; i++)
  {
    Asteroid *ast = &this->asteroids[i];
    if (!ast->isKilled)
    {
      drawCircle(renderBuffer, 
        *this->bottomLeftToTopLeftTransform * ast->translate,
        ast->collisionRadius, 
        COLOR_COLLISION_CIRCLE);        
    }
  }
#endif
}