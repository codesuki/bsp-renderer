#include "PlayerPhysicsComponent.h"

#include "util.h"
#include "World.h"

extern World world;

PlayerPhysicsComponent::PlayerPhysicsComponent(Entity& entity) : 
  entity_(entity), position_(entity.position_), 
  orientation_(entity.orientation_), cmds_(entity.cmds_), 
  up_(entity.up_), right_(entity.right_), look_(entity.look_)
{
}


PlayerPhysicsComponent::~PlayerPhysicsComponent(void)
{
}

void PlayerPhysicsComponent::Update(void)
{
  float speed = .001f * 10;//difference_;

  glm::vec4 accel; // add gravity + friction + air_friction

  accel += right_ * speed * cmds_.right_move;
  accel += look_ * speed * cmds_.forward_move;

  // can fly..? lol
  if (cmds_.up_move > 0) accel.z = 260;

  // trace to ground and get plane
  glm::vec4 wish_position = position_ + accel;
  glm::vec4 end = wish_position;
  glm::vec4 start = position_;

  glm::vec4 ground = start;

  start.z += 0.5f;
  ground.z -= 0.30f;

  glm::vec4 plane;

  float ground_fraction = world.map_.trace(start, ground, &plane);
  start.z -= 0.5f;

  bool on_ground = false;

  // we hit ground
  if (ground_fraction < 1.0f && accel.y > 0.0f)
  {
    on_ground = true;
    float distance = glm::dot(plane, accel);
    accel = accel - plane * distance;
    wish_position = position_ + accel;
    end = wish_position;
  }

  float fraction = 1.0f;
  
  //if (!g_noclip) 
  {
    accel.z += -9.8f;
    fraction = world.map_.trace(start, end, &plane);
  }

  // + velocity... velocity += accel
  position_ += accel * fraction;
}
