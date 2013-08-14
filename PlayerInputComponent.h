#pragma once

#include "Component.h"
#include "Entity.h"

class PlayerInputComponent : public Component
{
public:
  PlayerInputComponent(Entity& entity);
  ~PlayerInputComponent(void);

  void ReceiveMessage(Message& message){};
  void Update(unsigned int time){};

  int MoveUp(Message& msg) { entity_.cmds_.forward_move += 1; };
  int MoveDown(Message& msg) { entity_.cmds_.forward_move -= 1; };
  int MoveLeft(Message& msg) { entity_.cmds_.right_move += 1; };
  int MoveRight(Message& msg) { entity_.cmds_.right_move -= 1; };
  int MouseMove(MouseMoveMessage& msg) { entity_.cmds_.mouse_dx = msg.dx_; entity_.cmds_.mouse_dy = msg.dy_; };

private:
  Entity& entity_;
};

