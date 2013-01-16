#include "camera.h"
#include "bsp.h"

extern bool g_noclip;

camera::camera(glm::vec4& position, bsp* map) : position_(position), yaw_(0.0f), pitch_(0.0f), map_(map)
{
}

camera::~camera(void)
{
}

void camera::move(float dir)
{
  dir = dir * difference_;

  if (!g_noclip)
  {
    glm::vec4 wish_position = position_ + look_ * dir;

    glm::vec4 end = wish_position * quake2ogl;
    glm::vec4 start = position_ * quake2ogl;

    //std::cout << "wishpos before: " << wish_position << std::endl;
    float fraction = map_->trace(start, end);
    if (fraction != 1)
    {
      //std::cout << "wishpos after: " << wish_position * fraction << " fraction: " << fraction << std::endl;
      return;
    }
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
    
    glm::vec4 end = wish_position * quake2ogl;
    glm::vec4 start = position_ * quake2ogl;

    //std::cout << "wishpos before: " << wish_position << std::endl;
    float fraction = map_->trace(start, end);
    if (fraction != 1)
    { 
      //std::cout << "wishpos after: " << wish_position * fraction << " fraction: " << fraction << std::endl;
      return;
    }
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
  right_ = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  up_ = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  look_ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

  glm::mat4 matrix(1.0f);

  matrix = glm::rotate(matrix, yaw_, glm::vec3(up_));

  right_ = right_ * matrix;
  look_ = look_ * matrix;

  matrix = glm::mat4(1.0f);
  matrix = glm::rotate(matrix, pitch_, glm::vec3(right_));

  look_ = look_ * matrix;
  up_ = up_ * matrix;

  return glm::mat4(right_.x, up_.x, look_.x, 0.0f, 
    right_.y, up_.y, look_.y, 0.0f,
    right_.z, up_.z, look_.z, 0.0f, 
    glm::dot(-position_, right_), glm::dot(-position_, up_), glm::dot(-position_, look_), 1.0f);
}
