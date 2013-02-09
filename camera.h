#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "util.h"

struct cmd_t 
{
  char forward_move;
  char right_move;
  char up_move;
};

class bsp;

class camera
{
  public:
    camera(void);
    ~camera(void);

    camera(glm::vec4& position, bsp *map);

    void pitch(float pitch);
    void yaw(float yaw);
    glm::mat4 GetMatrix();

    void strafe(float dir);
    void move(float dir);

    void updateTime(float time);

    void update();

    glm::vec4 up_, right_, look_, position_;
    float difference_, pitch_, yaw_;
    bsp *map_;
};

#endif /* _CAMERA_H_ */

