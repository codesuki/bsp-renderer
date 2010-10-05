#include "camera.h"

camera::camera(vec3f* position) : position_(*position)
{
}

camera::~camera(void)
{
}

void camera::move(float dir)
{
  dir = dir * difference_;

  vec3f direction;
  direction = look_ - position_;
  direction.normalize();
  position_ += direction*dir;
  look_ += direction*dir;
}

void camera::strafe(float dir)
{
  dir = dir * difference_;

  vec3f direction;
  direction = look_ - position_;
  direction.normalize();
  direction = direction.cross(up_);
  position_ += direction*dir;
  look_ += direction*dir;
}

void camera::updateTime(float time)
{
  difference_ = time;
  
  right_.set(1, 0, 0);
  up_.set(0, 1, 0);
  look_.set(0, 0, 1);
}

#define PRINT_VAR(x) std::cout << #x << " " << x << std::endl

void camera::pitch(float pitch)
{
  PRINT_VAR(pitch_);
  pitch_ += pitch;
}

void camera::yaw(float yaw)
{
  PRINT_VAR(yaw_);
  yaw_ += yaw;
}

mat4f camera::GetMatrix()
{
  mat4f matrix;
  identity(matrix);
  matrix.rotate_z(yaw_);

  right_ = matrix * right_;
  look_ = matrix * look_;

  identity(matrix);
  matrix.rotate_x(pitch_);

  up_ = matrix * up_;
  look_ = matrix * look_;

  identity(matrix);
  matrix[0][0] = right_[0];
  matrix[0][1] = right_[1];
  matrix[0][2] = right_[2];

  matrix[1][0] = up_[0];
  matrix[1][1] = up_[1];
  matrix[1][2] = up_[2];

  matrix[2][0] = look_[0];
  matrix[2][1] = look_[1];
  matrix[2][2] = look_[2];

  matrix[3][0] = -position_.dot(right_);
  matrix[3][1] = -position_.dot(up_);
  matrix[3][2] = -position_.dot(look_);

  return matrix;
}
