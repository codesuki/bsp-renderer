#include "camera.h"
#include "bsp.h"

extern bool g_noclip;

camera::camera(glm::vec4& position, bsp* map) : position_(position), yaw_(0.0f), pitch_(0.0f), map_(map)
{
}

camera::~camera(void)
{
}

void camera::update()
{

}

void camera::move(float dir)
{
  //dir = dir * difference_;

  //if (!g_noclip)
  //{
  //  glm::vec4 wish_position = position_ + look_ * dir;

  //  glm::vec4 end = wish_position;
  //  glm::vec4 start = position_;

  //  float fraction = map_->trace(start, end, &start);
  //  position_ += look_ * dir * fraction;
  //}
  //else
  //{
  //  position_ += look_*dir;
  //}
}

void camera::strafe(float dir)
{
  //dir = dir * difference_;

  //if (!g_noclip)
  //{  
  //  glm::vec4 wish_position = position_ + right_ * dir;  
  //  
  //  glm::vec4 end = wish_position;
  //  glm::vec4 start = position_;

  //  float fraction = map_->trace(start, end, &start);
  //  position_ += right_ * dir * fraction; 
  //}
  //else
  //{
  //  position_ += right_ * dir;
  //}
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
  return glm::mat4();
}
