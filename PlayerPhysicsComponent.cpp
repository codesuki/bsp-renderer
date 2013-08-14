#include "PlayerPhysicsComponent.h"

#include "util.h"
#include "World.h"

extern World world;
extern cmd_t g_cmds;

PlayerPhysicsComponent::PlayerPhysicsComponent(Entity& entity) : 
  entity_(entity), position_(entity.position_), 
  orientation_(entity.orientation_), cmds_(entity.cmds_), 
  up_(entity.up_), right_(entity.right_), look_(entity.look_), pitch_(entity.pitch_), yaw_(entity.yaw_)
{
}

PlayerPhysicsComponent::~PlayerPhysicsComponent(void)
{
}

void PlayerPhysicsComponent::Update(unsigned int time)
{
  pitch_ += g_cmds.mouse_dy;
  if (pitch_ > 1.5707f) 
      pitch_ = 1.5707f;

  if (pitch_ < -1.5707f)
      pitch_ = -1.5707f;

  yaw_ += g_cmds.mouse_dx;
  if (yaw_ > 2*3.1415f)
      yaw_ = 0.0f;
  if (yaw_ < 2*-3.1415f)
      yaw_ = 0.0f;

  float speed = .001f * 70;//difference_;

  glm::vec4 accel; // add gravity + friction + air_friction

  accel += right_ * speed * g_cmds.right_move;
  accel += look_ * speed * g_cmds.forward_move;

  // can fly..? lol
  if (g_cmds.up_move > 0) accel.z = 260;

  // trace to ground and get plane
  glm::vec4 wish_position = position_ + accel;
  glm::vec4 end = wish_position;
  glm::vec4 start = position_;

  //glm::vec4 ground = start;

  ////start.z += 1.5f;
  //ground.z -= 21.0f;

  glm::vec4 plane;

  //float ground_fraction = world.map_->trace(start, ground, &plane);
  ////start.z -= 1.5f;

  //bool on_ground = false;

  //// we hit ground
  //if (ground_fraction < 1.0f && accel.z > 0.0f)
  //{
  //  on_ground = true;
  //  float distance = glm::dot(plane, accel);
  //  accel = accel - plane * distance;
  //  wish_position = position_ + accel;
  //  end = wish_position;
  //}

  float fraction = 1.0f;
  
  if (!entity_.noclip_) 
  {
    //accel.z += -9.8f;
    fraction = world.map_->trace(start, end, &plane);
  }

  // + velocity... velocity += accel
  position_ += accel * fraction;

  memset(&cmds_, 0, sizeof(cmd_t));
}
