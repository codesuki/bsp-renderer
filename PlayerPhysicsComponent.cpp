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
  // this goes into input component!
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

  //Accelerate();

  //GroundTrace();
    // trace to ground and get plane
  glm::vec4 start = position_;
  glm::vec4 ground_plane;
  glm::vec4 ground = start;
  ground.z -= 10.0f - 0.25f;

  float ground_fraction = world.map_->trace(start, ground, &ground_plane);

  bool on_ground = false;
  if (ground_fraction < 1.0f)   // we hit ground
  {
    on_ground = true;
  }

  // set boolean for jump key reset
  if (g_cmds.up_move > 0 && on_ground) accel.z = 50.0f;

  float fraction = 0.0f;

  if (entity_.noclip_) 
  {
    //NoClipMove();
    fraction = 1.0f;
  }
  //else if (!on_ground)
  //{
  //  //AirMove();
  //  //GroundMove();
  //}
  else
  {
    //GroundMove();
      accel.z += -9.8f;

  glm::vec4 wish_position;
  glm::vec4 plane;

  unsigned int bump_count;
  for (bump_count = 0; bump_count < 4; ++bump_count)
  {
    wish_position = position_ + accel;
    fraction = world.map_->trace(start, wish_position, &plane);
    if (fraction == 1.0f)
    {
      break;
    }

    float distance = glm::dot(plane, accel);
    accel = accel - plane * distance;

    wish_position = position_ + accel;    
  }
  }

  position_ += accel * fraction;
}

//int NoClipMove()
//{
//}
//
//int GroundMove()
//{
//}

//int GroundTrace()
//{
  //// trace to ground and get plane
  //glm::vec4 start = position_;
  //glm::vec4 ground_plane;
  //glm::vec4 ground = start;
  //ground.z -= 10.0f - 0.25f;

  //float ground_fraction = world.map_->trace(start, ground, &ground_plane);

  //bool on_ground = false;
  //if (ground_fraction < 1.0f)   // we hit ground
  //{
  //  on_ground = true;
  //}
//}
//
//int SlideMove(glm::vec4 accel)
//{
  //accel.z += -9.8f;

  //glm::vec4 wish_position;
  //glm::vec4 plane;

  //unsigned int bump_count;
  //for (bump_count = 0; bump_count < 4; ++bump_count)
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
//int SlideMoveStep()
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