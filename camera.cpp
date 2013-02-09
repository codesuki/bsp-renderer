#include "camera.h"
#include "bsp.h"

extern bool g_noclip;

camera::camera(glm::vec4& position, bsp* map) : position_(position), yaw_(0.0f), pitch_(0.0f), map_(map)
{
}

camera::~camera(void)
{
}

extern cmd_t cmds;

void camera::update()
{
  float speed = .001f * difference_;

  glm::vec4 accel; // add gravity + friction + air_friction

  accel += right_ * speed * cmds.right_move;
  accel += look_ * speed * cmds.forward_move;

  if (cmds.up_move > 0) accel.z = 260;

  // trace to ground and get plane

  glm::vec4 wish_position = position_ + accel;
  glm::vec4 end = wish_position;
  glm::vec4 start = position_;

  glm::vec4 ground = start;

  start.z += 0.5f;
  ground.z -= 0.30f;

  glm::vec4 plane;

  float ground_fraction = map_->trace(start, ground, &plane);
  start.z -= 0.5f;

  bool on_ground = false;

  // we hit ground
  if (ground_fraction < 1.0f && accel.y > 0.0f)
  {
    on_ground = true;
    float distance = glm::dot(plane, accel);
    accel = accel - plane*distance;
    wish_position = position_ + accel;
    end = wish_position;
  }

  float fraction = 1.0f;
  
  if (!g_noclip) 
  {
    accel.z += -9.8f;
    fraction = map_->trace(start, end, &plane);
  }

  // + velocity... velocity += accel
  position_ += accel * fraction;
}

void camera::move(float dir)
{
  dir = dir * difference_;

  if (!g_noclip)
  {
    glm::vec4 wish_position = position_ + look_ * dir;

    glm::vec4 end = wish_position;
    glm::vec4 start = position_;

    float fraction = map_->trace(start, end, &start);
    position_ += look_ * dir * fraction;
  }
  else
  {
    position_ += look_*dir;
  }
}

void camera::strafe(float dir)
{
  dir = dir * difference_;

  if (!g_noclip)
  {  
    glm::vec4 wish_position = position_ + right_ * dir;  
    
    glm::vec4 end = wish_position;
    glm::vec4 start = position_;

    float fraction = map_->trace(start, end, &start);
    position_ += right_ * dir * fraction; 
  }
  else
  {
    position_ += right_ * dir;
  }
}

void camera::updateTime(float time)
{
  difference_ = time;
}

void camera::pitch(float pitch)
{
  pitch_ += pitch;
  if (pitch_ > 1.5707f) 
      pitch_ = 1.5707f;

  if (pitch_ < -1.5707f)
      pitch_ = -1.5707f;
}

void camera::yaw(float yaw)
{
  yaw_ += yaw;
  if (yaw_ > 2*3.1415f)
      yaw_ = 0.0f;
  if (yaw_ < 2*-3.1415f)
      yaw_ = 0.0f;
}

glm::mat4 camera::GetMatrix()
{
  // this is quake3 coordinate system.
  look_ = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  right_ = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  up_ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

  glm::mat4 matrix(1.0f);

  matrix = glm::rotate(matrix, yaw_, glm::vec3(up_));

  right_ = matrix * right_;
  look_ = matrix * look_;

  matrix = glm::mat4(1.0f);
  matrix = glm::rotate(matrix, pitch_, glm::vec3(right_));

  look_ = matrix * look_;
  up_ = matrix * up_;

  // switch to opengl coordinate system for view matrix calculation
  glm::vec4 right_ogl = quake2ogl * right_;
  glm::vec4 look_ogl = quake2ogl * look_;
  glm::vec4 up_ogl = quake2ogl * up_;
  glm::vec4 position_ogl = quake2ogl * position_;

    //return glm::mat4(look_.x, right_.x, up_.x, 0.0f, 
    //look_.y, right_.y, up_.y, 0.0f,
    //look_.z, right_.z, up_.z, 0.0f, 
    //glm::dot(-position_, look_), glm::dot(-position_, right_), glm::dot(-position_, up_), 1.0f);

  return glm::mat4(right_ogl.x, up_ogl.x, look_ogl.x, 0.0f, 
    right_ogl.y, up_ogl.y, look_ogl.y, 0.0f,
    right_ogl.z, up_ogl.z, look_ogl.z, 0.0f, 
    glm::dot(-position_ogl, right_ogl), glm::dot(-position_ogl, up_ogl), glm::dot(-position_ogl, look_ogl), 1.0f);
}
