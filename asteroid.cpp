#include "zzxoto_asteroid.h"
#include "zzxoto_constants.h"

global Model g_asteroidModel;
global bool32 g_isAsteroidModelInitialized = false;

Asteroid::Asteroid(Mat3 *bottomLeftToTopLeftTransform, int asteroidType, V2 translate)
{
  if (!g_isAsteroidModelInitialized)
  {
    int V = 9;

    g_asteroidModel.indicesCount = V + 1;
    g_asteroidModel.vertices = (V2 *) platform_malloc(sizeof(V2) * V);
    g_asteroidModel.colors = (int *) platform_malloc(sizeof(int) * V);
    g_asteroidModel.indices = (int *) platform_malloc(sizeof(int) * (V + 1));
    g_asteroidModel.drawType = DRAW_TYPE_POLYGON;
    g_asteroidModel.transform = identity();

    for (int i = 0; i < V; i++)
    {
      g_asteroidModel.colors[i] = COLOR_ASTEROID;
      g_asteroidModel.indices[i] = i;
    }
    g_asteroidModel.indices[V] = 0;

    g_asteroidModel.vertices[0] = {-.3f, .5f};
    g_asteroidModel.vertices[1] = {-.4f, .4f};
    g_asteroidModel.vertices[2] = {-.3f, .3f};
    g_asteroidModel.vertices[3] = {-.5f, -.1f};
    g_asteroidModel.vertices[4] = {-.1f, -.3f};
    g_asteroidModel.vertices[5] = {-.1f, -.2f};
    g_asteroidModel.vertices[6] = {.3f, -.4f};
    g_asteroidModel.vertices[7] = {.2f, .1f};
    g_asteroidModel.vertices[8] = {.3f, .3f};

    g_isAsteroidModelInitialized = true;
  }

  float roll = platform_rand();

  this->asteroidType = asteroidType;
  switch(asteroidType)
  {
    case ASTEROID_TYPE_SMALL:
    {
      this->scale = ASTEROID_SCALE_SMALL;
      this->collisionRadius = ASTEROID_COLLISION_RADIUS_SMALL;
      this->score = ASTEROID_SCORE_SMALL;
    }
    break;
    case ASTEROID_TYPE_MEDIUM:
    {
      this->scale = ASTEROID_SCALE_MEDIUM;
      this->collisionRadius = ASTEROID_COLLISION_RADIUS_MEDIUM;
      this->score = ASTEROID_SCORE_MEDIUM;
    }
    break;
    default:
    {
      this->scale = ASTEROID_SCALE_LARGE;
      this->collisionRadius = ASTEROID_COLLISION_RADIUS_LARGE;
      this->score = ASTEROID_SCORE_LARGE;
    }
  }

  this->angularVelocityPerSecond = platform_rand<float>(ASTEROID_MIN_ANGULAR_VELOCITY_PS, ASTEROID_MAX_ANGULAR_VELOCITY_PS);

  float velocity = platform_rand<float>(ASTEROID_MIN_VELOCITY, ASTEROID_MAX_VELOCITY);
  this->velocity.x = velocity * roll;
  this->velocity.y = velocity * (1 - roll);
  if (roll < 0.5f)
  {
    this->velocity.x *= -1;
    this->velocity.y *= -1;
  }

  //initial orientation
  this->orientation = platform_rand() * 360.f;
  if (roll < 0.5f)
  {
    this->orientation *= -1.f;
  }

  this->translate = translate;
  this->model = g_asteroidModel;
  this->bottomLeftToTopLeftTransform = bottomLeftToTopLeftTransform;
  this->isKilled = false;
}

void Asteroid::kill()
{
  this->isKilled = true;
}

void Asteroid::update(platform_GameInput *gameInput)
{
  Asteroid *ast = this;
  float angularVelocity = ast->angularVelocityPerSecond * gameInput->dtForFrame;
  ast->orientation = angularVelocity + ast->orientation;
  float deltaX = ast->velocity.x * gameInput->dtForFrame;    
  float deltaY = ast->velocity.y * gameInput->dtForFrame;
  ast->translate.x += deltaX;
  ast->translate.y += deltaY;
  ast->translate.x = wrap<float>(ast->translate.x, GAME_WIDTH - 1);
  ast->translate.y = wrap<float>(ast->translate.y, GAME_HEIGHT - 1);
  ast->model.transform = *ast->bottomLeftToTopLeftTransform * scaleRotateTranslate(ast->scale, ast->orientation, ast->translate);
}

void Asteroid::render(platform_RenderBuffer *renderBuffer)
{
  drawModel_outline(renderBuffer, &this->model);
}