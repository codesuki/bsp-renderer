#pragma once
#include "component.h"
#include "Entity.h"

class PlayerAnimationComponent : public Component
{
public:
  PlayerAnimationComponent(Entity& entity);
  ~PlayerAnimationComponent(void);

  void ReceiveMessage(Message& message){};
  void Update(unsigned int time);

  Entity& entity_;
  unsigned int& upper_frame_;
  unsigned int& lower_frame_;
};

