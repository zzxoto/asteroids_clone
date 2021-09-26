#ifndef ZZXOTO_PLAYER_H
#define ZZXOTO_PLAYER_H
#include "zzxoto_platform.h"
#include "zzxoto_renderer.h"

struct Bullet;
struct Player;

struct Player
{
  V2 velocity;
  float maxVelocity;
  float angularVelocityPerSecond;
  float scale;
  float orientation;
  V2 translate;
  Model model;

  Bullet *bullets;
  int bulletListSize;
  float lastBulletFired;

  Mat3 *bottomLeftToTopLeftTransform;

  float collisionRadius;

  bool32 isKilled;

  Player(Mat3 *bottomLeftToTopLeftTransform, const Model &model);

  void update(platform_GameInput *gameInput);
  void render(platform_RenderBuffer *renderBuffer);
  void kill();
};

struct Bullet
{
  V2 velocity;
  V2 translate;
  float secondsInExistence;
  float collisionRadius;
  Mat3 *bottomLeftToTopLeftTransform;

  bool32 isKilled;

  Bullet(Mat3 *bottomLeftToTopLeftTransform, V2 velocity, V2 translate)
  {
    init(bottomLeftToTopLeftTransform, velocity, translate);
  }
  
  void update(platform_GameInput *gameInput);
  void render(platform_RenderBuffer *renderBuffer);
  void kill();
private:
  void init(Mat3 *bottomLeftToTopLeftTransform, V2 velocity, V2 translate);
};

#endif