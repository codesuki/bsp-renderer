#include "player_animation_component.hpp"

#include "entity.hpp"

#include "logger.hpp"

PlayerAnimationComponent::PlayerAnimationComponent()
    : model_("models/players/", "visor") {
  upper_current_animation_ = TORSO_STAND;
  lower_current_animation_ = LEGS_WALK;
}

PlayerAnimationComponent::~PlayerAnimationComponent(void) {}

void PlayerAnimationComponent::Update(unsigned int time) {
  /*  int skip = 153 - 90;
  int g_lower_fps = 15;
  int g_lower_startFrame = 220 - skip;
  int g_lower_numFrames = 10;
  */

  animation_info *lower_animation =
      &model_.animations_[lower_current_animation_];

  auto g_lower_fps = lower_animation->frames_per_second;
  auto g_lower_startFrame = lower_animation->first_frame;
  auto g_lower_numFrames = lower_animation->num_frames;

  logger::Debug("lower animation: %f %d %d", g_lower_fps, g_lower_startFrame,
                g_lower_numFrames);

  static auto fLowerLastTime = 0.0f;
  static auto lowerFrame = 0u;

  fLowerLastTime += time;
  if (fLowerLastTime > g_lower_fps) {
    ++lowerFrame;
    if (lowerFrame >= g_lower_numFrames) {
      lowerFrame = 0;
    }

    if (lower_animation->reversed) {
      entity_->lower_frame =
          g_lower_startFrame + g_lower_numFrames - 1 - lowerFrame;
    } else {
      entity_->lower_frame = g_lower_startFrame + lowerFrame;
    }
    /*
    ++entity_->lower_frame;
    if (entity_->lower_frame >= g_lower_startFrame + g_lower_numFrames) {
      entity_->lower_frame = g_lower_startFrame;
      }*/
    fLowerLastTime = 0.0;
  }

  /*
  int g_upper_fps = 20;
  int g_upper_startFrame = 90;
  int g_upper_numFrames = 40;
  */

  animation_info *upper_animation =
      &model_.animations_[upper_current_animation_];

  float g_upper_fps = upper_animation->frames_per_second;
  int g_upper_startFrame = upper_animation->first_frame;
  int g_upper_numFrames = upper_animation->num_frames;

  static float fUpperLastTime = 0.0f;

  fUpperLastTime += time;
  if (fUpperLastTime > g_upper_fps) {
    ++entity_->upper_frame;
    if (entity_->upper_frame >= g_upper_startFrame + g_upper_numFrames) {
      entity_->upper_frame = g_upper_startFrame;
    }
    fUpperLastTime = 0.0;
  }
}
