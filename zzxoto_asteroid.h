#ifndef ZZXOTO_ASTEROID_H
#define ZZXOTO_ASTEROID_H
#include "zzxoto_platform.h"
#include "zzxoto_renderer.h"

struct Asteroid
{
  V2 velocity;
  float angularVelocityPerSecond;
  float scale;
  float orientation;
  V2 translate;
  Model model;
  float collisionRadius;
  int asteroidType;
  int score;

  Mat3 *bottomLeftToTopLeftTransform;

  bool32 isKilled;

  Asteroid(Mat3 *bottomLeftToTopLeftTransform, int asteroidType, V2 translate);

  void update(platform_GameInput *gameInput);
  void render(platform_RenderBuffer *renderBuffer);
  void kill();
};
#endif