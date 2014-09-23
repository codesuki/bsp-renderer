#include "player_physics_component.hpp"

#include "world.hpp"
#include "input.hpp"
#include "entity.hpp"
#include "bsp.hpp"

// TODO: get rid of those globals (pass to method)
extern World world;
extern cmd_t g_cmds;

PlayerPhysicsComponent::PlayerPhysicsComponent()
{
}

PlayerPhysicsComponent::~PlayerPhysicsComponent()
{
}

// TODO: should be in input component
// TODO: remove magic numbers
void PlayerPhysicsComponent::UpdatePitch()
{
  auto& pitch = entity_->pitch_;
  
  pitch += g_cmds.mouse_dy;
  if (pitch > 1.5707f)
  {
    pitch = 1.5707f;
  }
  if (pitch < -1.5707f)
  {
    pitch = -1.5707f;
  }
}

void PlayerPhysicsComponent::UpdateYaw()
{
  auto& yaw = entity_->yaw_;
  
  yaw += g_cmds.mouse_dx;
  if (yaw > 2*3.1415f)
  {
    yaw = 0.0f;
  }
  if (yaw < 2*-3.1415f)
  {
    yaw = 0.0f;
  }
}

void PlayerPhysicsComponent::Update(unsigned int time)
{
  UpdatePitch();

  UpdateYaw();
  
  float speed = .001f * 70;//difference_;

  glm::vec4 accel; // add gravity + friction + air_friction
  accel += entity_->right_ * speed * static_cast<float>(g_cmds.right_move);
  accel += entity_->look_ * speed * static_cast<float>(g_cmds.forward_move);

  //Accelerate();

  //GroundTrace();
    // trace to ground and get plane
  glm::vec4 start = entity_->position_;
  glm::vec4 ground_plane;
  glm::vec4 ground = start;
  ground.z -= 10.0f - 0.25f; // this are q3 coordinates

  float ground_fraction = world.map_->trace(start, ground, &ground_plane);

  bool on_ground = false;
  if (ground_fraction < 1.0f)   // we hit ground
  {
    on_ground = true;
  }

  // set boolean for jump key reset
  if (g_cmds.up_move > 0 && on_ground) accel.z = 50.0f;

  float fraction = 0.0f;

  if (entity_->noclip_) 
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
      wish_position = entity_->position_ + accel;
      fraction = world.map_->trace(start, wish_position, &plane);
      if (fraction == 1.0f)
      {
        break;
      }
      
      float distance = glm::dot(plane, accel);
      accel = accel - plane * distance;
      
      wish_position = entity_->position_ + accel;    
    }
  }

  entity_->position_ += accel * fraction;
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
