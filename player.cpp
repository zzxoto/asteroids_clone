#include "zzxoto_player.h"
#include "zzxoto_constants.h"

Player::Player(Mat3 *bottomLeftToTopLeftTransform, const Model &model)
{
  this->velocity = { 0.f, 0.f };
  this->maxVelocity = PLAYER_MAX_VELOCITY;
  this->angularVelocityPerSecond = PLAYER_ANGULAR_VELOCITY_PS;
  this->bottomLeftToTopLeftTransform = bottomLeftToTopLeftTransform;
  this->model = model;
  this->scale = PLAYER_SCALE;
  this->orientation = 0.f;
  this->translate = { 100.f, 20.f };
  this->collisionRadius = PLAYER_COLLISION_RADIUS;

  this->bulletListSize = 20;
  this->lastBulletFired = 0.f;
  this->bullets = (Bullet *) platform_malloc(sizeof(Bullet) * this->bulletListSize);
  for (int i = 0; i < bulletListSize; i++)
  {
    this->bullets[i].isKilled = true;
  }
}

void Player::kill()
{
  this->isKilled = true;

  for (int i = 0; i < this->bulletListSize; i++)
  {
    Bullet *b = &this->bullets[i];
    if (!b->isKilled)
    {
      b->kill();
    }
  }
  platform_free(this->bullets);
}

void Player::render(platform_RenderBuffer *renderBuffer)
{
  drawModel_outline(renderBuffer, &this->model);

  for (int i = 0; i < this->bulletListSize; i++)
  {
    Bullet *bullet = this->bullets + i;
    if (!bullet->isKilled)
    {
      bullet->render(renderBuffer);
    }    
  }
}

void Player::update(platform_GameInput *gameInput)
{
  float angularVelocity = this->angularVelocityPerSecond * gameInput->dtForFrame;
  if (gameInput->leftIsDown)
  {
    angularVelocity *= 1;
  }
  else if (gameInput->rightIsDown)
  {
    angularVelocity *= -1;
  }
  else
  {
    angularVelocity = 0;
  }
  this->orientation += angularVelocity;

  float accelaration = 0;
  if (gameInput->upIsDown)
  {
    accelaration = this->maxVelocity * 4;
  }

  Mat3 rotationMatrix = rotate(this->orientation);
  V3 accelarationDirection = { 0.f, 1.f, 1.f };
  accelarationDirection = (rotationMatrix * accelarationDirection);
  float accelarationX = accelarationDirection.x * accelaration;
  float accelarationY = accelarationDirection.y * accelaration;

  float deltaX = (.5 * square(gameInput->dtForFrame) * accelarationX) + (this->velocity.x * gameInput->dtForFrame);
  float deltaY = (.5 * square(gameInput->dtForFrame) * accelarationY) + (this->velocity.y * gameInput->dtForFrame);
  this->translate = { this->translate.x + deltaX, this->translate.y + deltaY };
  this->translate.x = wrap<float>(this->translate.x, GAME_WIDTH - 1);
  this->translate.y = wrap<float>(this->translate.y, GAME_HEIGHT - 1);

  this->velocity.x += accelarationX * gameInput->dtForFrame;
  this->velocity.y += accelarationY * gameInput->dtForFrame;

  float velocityMagnitude = magnitude(this->velocity);
  if (velocityMagnitude > this->maxVelocity)
  {
    float factor = velocityMagnitude / this->maxVelocity;
    this->velocity = divide(this->velocity, factor);
  }

  this->model.transform = *this->bottomLeftToTopLeftTransform * scaleRotateTranslate(this->scale, this->orientation, this->translate);

  if (gameInput->spaceIsDown)
  {
    if ((gameInput->secondsElapsed - this->lastBulletFired) >= BULLET_FIRE_FREQUENCY)
    {
      this->lastBulletFired = gameInput->secondsElapsed;
      for (int i = 0; i < this->bulletListSize; i++)
      {
        Bullet *bullet = this->bullets + i;
        if (bullet->isKilled)
        {
          V3 direction = { 0.f, 1.f, 1.f };
          direction = (rotationMatrix * direction);
          V2 bulletVelocity = { direction.x * BULLET_VELOCITY, direction.y * BULLET_VELOCITY };
          *bullet = Bullet(this->bottomLeftToTopLeftTransform, bulletVelocity, this->translate);
          break;
        }
      }
    }
  }

  for (int i = 0; i < this->bulletListSize; i++)
  {
    Bullet *bullet = this->bullets + i;
    if (!bullet->isKilled)
    {
      bullet->update(gameInput);
    }
  }
}

void Bullet::init(Mat3 *bottomLeftToTopLeftTransform, V2 velocity, V2 translate)
{
  this->isKilled = false;
  this->secondsInExistence = 0.f;
  this->translate = translate;
  this->velocity = velocity;
  this->collisionRadius = BULLET_COLLISION_RADIUS;
  this->bottomLeftToTopLeftTransform = bottomLeftToTopLeftTransform;
}

void Bullet::update(platform_GameInput *gameInput)
{
  if (this->secondsInExistence > BULLET_LIFE_SECONDS_MAX)
  {
    this->kill();
  }
  else
  {
    float tx = this->translate.x + (this->velocity.x * gameInput->dtForFrame);
    float ty = this->translate.y + (this->velocity.y * gameInput->dtForFrame);
    tx = wrap<float>(tx, GAME_WIDTH - 1);
    ty = wrap<float>(ty, GAME_HEIGHT - 1);
    this->translate = { tx, ty };
    this->secondsInExistence += gameInput->dtForFrame;
  }
}

void Bullet::render(platform_RenderBuffer *renderBuffer)
{
  V2 position = *this->bottomLeftToTopLeftTransform * this->translate;
  drawDot(renderBuffer, position, BULLET_THICKNESS, COLOR_BULLET);
}

void Bullet::kill()
{
  this->isKilled = true;
}