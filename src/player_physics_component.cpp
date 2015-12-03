#include "player_physics_component.hpp"

#include "logger.hpp"
#include "world.hpp"
#include "input.hpp"
#include "entity.hpp"
#include "bsp.hpp"

// TODO: get rid of those globals (pass to method)
extern World world;
extern cmd_t g_cmds;

PlayerPhysicsComponent::PlayerPhysicsComponent() {}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {}

// TODO: should be in input component
// TODO: remove magic numbers
void PlayerPhysicsComponent::UpdatePitch() {
  auto &pitch = entity_->pitch_;

  pitch += g_cmds.mouse_dy;
  if (pitch > 1.5707f) {
    pitch = 1.5707f;
  }
  if (pitch < -1.5707f) {
    pitch = -1.5707f;
  }
}

void PlayerPhysicsComponent::UpdateYaw() {
  auto &yaw = entity_->yaw_;

  yaw += g_cmds.mouse_dx;
  if (yaw > 2 * 3.1415f) {
    yaw = 0.0f;
  }
  if (yaw < 2 * -3.1415f) {
    yaw = 0.0f;
  }
}

void PlayerPhysicsComponent::Update(unsigned int time) {
  if (!is_active_) {
    return;
  }

  // is this actually correct?? should be applied to right and look vectors
  // because not it seems like they lag a frame behind
  UpdatePitch();
  UpdateYaw();

  float dt = static_cast<float>(time) * 0.001f;

  // GroundTrace();
  // trace to ground and get plane
  glm::vec4 start = entity_->position_;
  glm::vec4 ground_plane;
  glm::vec4 ground = start;
  ground.z -= 10.0f - 0.25f; // this are q3 coordinates

  float ground_fraction = world.map_->trace(start, ground, &ground_plane);

  bool on_ground = false;
  // we hit ground
  if (ground_fraction < 1.0f) {
    on_ground = true;
  }

  float speed = 0.35f;

  // ignore movement in Z direction or else we start flying
  // but not in spectator mode...
  glm::vec4 right = entity_->right_;
  right = glm::normalize(right);

  glm::vec4 look = entity_->look_;
  look = glm::normalize(look);

  glm::vec4 accel;
  accel += right * speed * static_cast<float>(g_cmds.right_move);
  accel += look * speed * static_cast<float>(g_cmds.forward_move);

  // accel.x = speed * static_cast<float>(g_cmds.right_move);
  // accel.y = speed * static_cast<float>(g_cmds.forward_move);

  // Accelerate();

  float fraction = 0.0f;

  if (entity_->noclip_) {
    // set boolean for jump key reset
    if (g_cmds.up_move > 0 && on_ground) {
      accel.z = 200;
    }
    // NoClipMove();
    fraction = 1.0f;
    entity_->velocity_ += (accel - 5.0f * entity_->velocity_) * dt;
    entity_->position_ += entity_->velocity_ * fraction;
  }
  // else if (!on_ground)
  //{
  //  //AirMove();
  //  //GroundMove();
  //}
  else {
    accel.z = 0.0f;
    // set boolean for jump key reset
    if (g_cmds.up_move > 0 && on_ground) {
      accel.z = 3000.0f;
    }
    // GroundMove();
    accel.z += -800.0f;

    // maybe we have to remove the ground-hitting velocity already?
    glm::vec4 wish_velocity =
        entity_->velocity_ + (accel - 6.0f * entity_->velocity_) * dt;

    glm::vec4 wish_position;
    glm::vec4 plane;

    unsigned int bump_count;
    for (bump_count = 0; bump_count < 4; ++bump_count) {
      wish_position = entity_->position_ + wish_velocity;
      fraction = world.map_->trace(start, wish_position, &plane);
      if (fraction == 1.0f) {
        break;
      }

      float distance = glm::dot(plane, wish_velocity);
      wish_velocity = wish_velocity - plane * distance;

      wish_position = entity_->position_ + wish_velocity;
    }

    entity_->velocity_ = wish_velocity;
    entity_->position_ = wish_position;
  }
}

// int NoClipMove()
//{
//}
//
// int GroundMove()
//{
//}

// int GroundTrace()
//{
//// trace to ground and get plane
// glm::vec4 start = position_;
// glm::vec4 ground_plane;
// glm::vec4 ground = start;
// ground.z -= 10.0f - 0.25f;

// float ground_fraction = world.map_->trace(start, ground, &ground_plane);

// bool on_ground = false;
// if (ground_fraction < 1.0f)   // we hit ground
//{
//  on_ground = true;
//}
//}
//
// int SlideMove(glm::vec4 accel)
//{
// accel.z += -9.8f;

// glm::vec4 wish_position;
// glm::vec4 plane;

// unsigned int bump_count;
// for (bump_count = 0; bump_count < 4; ++bump_count)
//{
//  wish_position = position_ + accel;
//  fraction = world.map_->trace(start, wish_position, &plane);
//  if (fraction == 1.0f)
//  {
//    break;
//  }

//  float distance = glm::dot(plane, accel);
//  accel = accel - plane * distance;

//  wish_position = position_ + accel;
//}
//
//
// int SlideMoveStep()
//{
//  int bumps = SlideMove();
//
//  if (bumps == 0)
//  {
//    return;
//  }
//
//  // step up stepsize
//  // trace / slidemove
//}
