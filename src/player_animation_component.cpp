#include "player_animation_component.hpp"

#include "entity.hpp"

PlayerAnimationComponent::PlayerAnimationComponent()
{
}

PlayerAnimationComponent::~PlayerAnimationComponent(void)
{
}

void PlayerAnimationComponent::Update(unsigned int time)
{
  time = time/60;
  int skip = 153 - 90;
  int g_lower_fps = 15;
  int g_lower_startFrame = 220 - skip;
  int g_lower_numFrames = 10;

  static float fLowerLastTime = 0.0f;

  if (time - fLowerLastTime > (1.0 / g_lower_fps)) 
  {
    ++entity_->lower_frame;
    if (entity_->lower_frame >= g_lower_startFrame+g_lower_numFrames)
    {
      entity_->lower_frame = g_lower_startFrame;
    }
    fLowerLastTime = time;
  }

  int g_upper_fps = 20;
  int g_upper_startFrame = 90;
  int g_upper_numFrames = 40;

  static float fUpperLastTime = 0.0f;

  if (time - fUpperLastTime > (1.0 / g_upper_fps)) 
  {
    ++entity_->upper_frame;
    if (entity_->upper_frame >= g_upper_startFrame+g_upper_numFrames)
    {
      entity_->upper_frame = g_upper_startFrame;
    }
    fUpperLastTime = time;
  }
}
