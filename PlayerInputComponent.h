#pragma once

#include "Component.h"
#include "Entity.h"

class PlayerInputComponent : public Component
{
public:
  PlayerInputComponent(Entity& entity);
  ~PlayerInputComponent(void);

  int ReceiveMessage(Message& message);

  int MoveUp(bool isDown) { entity_.cmds.move_up = isDown; };
  int MoveDown() {  };
  int MoveLeft() {  };
  int MoveRight() {  };

private:
  Entity& entity_;
};

