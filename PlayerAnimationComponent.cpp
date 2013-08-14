#include "PlayerAnimationComponent.h"


PlayerAnimationComponent::PlayerAnimationComponent(Entity& entity) : 
  entity_(entity), upper_frame_(entity.upper_frame), lower_frame_(entity.lower_frame)
{
  int skip = 153 - 90;
  int g_lower_fps = 18;
  int g_lower_startFrame = 173 - skip;

  lower_frame_ = g_lower_startFrame;
  upper_frame_ = 151;
}


PlayerAnimationComponent::~PlayerAnimationComponent(void)
{
}

void PlayerAnimationComponent::Update(unsigned int time)
{
  time = time/60;
  int skip = 153 - 90;
  int g_lower_fps = 18;
  int g_lower_startFrame = 173 - skip;
  int g_lower_numFrames = 9;

  static float fLowerLastTime = 0.0f;

  if (time - fLowerLastTime > (1.0 / g_lower_fps)) 
  {
    ++lower_frame_;
    if (lower_frame_ >= g_lower_startFrame+g_lower_numFrames) { lower_frame_ = g_lower_startFrame; }
    fLowerLastTime = time;
  }

  int g_upper_fps = 15;
  int g_upper_startFrame = 151;
  int g_upper_numFrames = 1;

  static float fUpperLastTime = 0.0f;

  if (time - fUpperLastTime > (1.0 / g_upper_fps)) 
  {
    ++upper_frame_;
    if (upper_frame_ >= g_upper_startFrame+g_upper_numFrames) { upper_frame_ = g_upper_startFrame; }
    fUpperLastTime = time;
  }
}
