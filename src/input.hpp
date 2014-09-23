#ifndef INPUT_H_
#define INPUT_H_

struct cmd_t 
{
  char forward_move;
  char right_move;
  char up_move;
  float mouse_dx;
  float mouse_dy;
};

namespace input 
{
  void Initialize();
  cmd_t Update();
}

#endif
