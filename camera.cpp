#include "camera.h"

camera::camera(void)
{
}

camera::~camera(void)
{
}

camera::camera(vec3d* position, vec3d* lookat, vec3d* up)
{
  m_position = *position;
  m_lookat = *lookat;
  m_up = *up;
}

void camera::move(float dir)
{
  dir = dir * m_difference;

  vec3d direction;
  direction = m_lookat - m_position;
  direction.normalize();
  m_position += direction*dir;
  m_lookat += direction*dir;
}

void camera::strafe(float dir)
{
  dir = dir * m_difference;

  vec3d direction;
  direction = m_lookat - m_position;
  direction.normalize();
  direction = direction.cross(m_up);
  m_position += direction*dir;
  m_lookat += direction*dir;
}

void camera::rotate(int x, int y)
{
  if (x == 0 && y == 0) return;
  float _x = (float)x;
  float _y = (float)y;
  _x /= 1000;
  _y /= 1000;

  vec3d direction;
  direction = m_lookat - m_position;
  direction.normalize();
  direction = direction.cross(m_up);

  rotate(_y, direction);
  rotate(-_x, m_up);
}

void camera::rotate(float angle, vec3f axis)
{
  vec3d direction;
  direction = m_lookat - m_position;
  direction.normalize();

  mat4d mY;
  mY.rotate(angle, axis);
  vec3d vY;
  vY = mY * direction;

  m_lookat = m_position + vY;
}

void camera::updateTime(float time)
{
  m_difference = time;
}
