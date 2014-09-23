#ifndef PLAYER_INPUT_COMPONENT_HPP_
#define PLAYER_INPUT_COMPONENT_HPP_

#include "component.hpp"
#include "entity.hpp"
#include "message.hpp"

class PlayerInputComponent : public Component
{
public:
  PlayerInputComponent();
  ~PlayerInputComponent(void);

  void ReceiveMessage(Message& message){};
  void Update(unsigned int time){};

  int MoveUp(Message& msg) { entity_->cmds_.forward_move += 1; };
  int MoveDown(Message& msg) { entity_->cmds_.forward_move -= 1; };
  int MoveLeft(Message& msg) { entity_->cmds_.right_move += 1; };
  int MoveRight(Message& msg) { entity_->cmds_.right_move -= 1; };
  int MouseMove(MouseMoveMessage& msg)
  {
    entity_->cmds_.mouse_dx = msg.dx_;
    entity_->cmds_.mouse_dy = msg.dy_;
  };
};

#endif

