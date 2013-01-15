#include "camera.h"
#include "bsp.h"

extern bool g_noclip;

camera::camera(vec3f* position, bsp* map) : position_(*position), yaw_(0.0f), pitch_(0.0f), map_(map)
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
    mat4f transform_matrix;
    transform_matrix = quake2oglMatrix;

    vec3f wish_position = position_ + look_ * dir;

    vec3f end = transform_matrix * wish_position;
    vec3f start = transform_matrix * position_;

    std::cout << "wishpos before: " << wish_position << std::endl;
    float fraction = map_->trace(start, end);
    if (fraction != 1)
    {
      std::cout << "wishpos after: " << wish_position * fraction << " fraction: " << fraction << std::endl;
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
    mat4f transform_matrix;
    transform_matrix = quake2oglMatrix;

    vec3f wish_position = position_ + right_ * dir;  
    
    vec3f end = transform_matrix * wish_position;
    vec3f start = transform_matrix * position_;

    std::cout << "wishpos before: " << wish_position << std::endl;
    float fraction = map_->trace(start, end);
    if (fraction != 1)
    { 
      std::cout << "wishpos after: " << wish_position * fraction << " fraction: " << fraction << std::endl;
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
  if (pitch_ > 1.5707) 
      pitch_ = 1.5707;

  if (pitch_ < -1.5707)
      pitch_ = -1.5707;
}

void camera::yaw(float yaw)
{
  yaw_ += yaw;
  if (yaw_ > 2*3.1415)
      yaw_ = 0;
  if (yaw_ < 2*-3.1415)
      yaw_ = 0;
}

mat4f camera::GetMatrix()
{
  right_.set(1, 0, 0);
  up_.set(0, 1, 0);
  look_.set(0, 0, 1);

  mat4f matrix;

  /*identity(matrix);
  matrix.rotate_z(0);

  right_ = matrix * right_;
  //look_ = matrix * look_;
  up_ = matrix * up_;
  */

  identity(matrix);
  matrix.rotate(yaw_, up_);

  right_ = matrix * right_;
  look_ = matrix * look_;

  identity(matrix);
  matrix.rotate(pitch_, right_);

  look_ = matrix * look_;
  up_ = matrix * up_;

  identity(matrix);
  
  matrix[(size_t)0][(size_t)0] = right_[(size_t)0];
  matrix[(size_t)0][(size_t)1] = right_[(size_t)1];
  matrix[(size_t)0][(size_t)2] = right_[(size_t)2];

  matrix[(size_t)1][(size_t)0] = up_[(size_t)0];
  matrix[(size_t)1][(size_t)1] = up_[(size_t)1];
  matrix[(size_t)1][(size_t)2] = up_[(size_t)2];

  matrix[(size_t)2][(size_t)0] = look_[(size_t)0];
  matrix[(size_t)2][(size_t)1] = look_[(size_t)1];
  matrix[(size_t)2][(size_t)2] = look_[(size_t)2];
                     
  matrix[(size_t)0][(size_t)3] = -position_.dot(right_);
  matrix[(size_t)1][(size_t)3] = -position_.dot(up_);
  matrix[(size_t)2][(size_t)3] = -position_.dot(look_);
   
 /* 
  matrix[0][0] = right_[0];
  matrix[0][1] = up_[0];
  matrix[0][2] = -look_[0];

  matrix[1][0] = right_[1];
  matrix[1][1] = up_[1];
  matrix[1][2] = -look_[1];

  matrix[2][0] = right_[2];
  matrix[2][1] = up_[2];
  matrix[2][2] = -look_[2];
                     
  matrix[0][3] = -position_.dot(right_);
  matrix[1][3] = -position_.dot(up_);
  matrix[2][3] = -position_.dot(look_);
   
  PRINT_VAR(position_);
  PRINT_VAR(right_);
  PRINT_VAR(up_);
  PRINT_VAR(look_);
  PRINT_VAR(pitch_);
  PRINT_VAR(yaw_);
  */
  return matrix;
}
