#ifndef PLAYER_ANIMATION_COMPONENT_HPP_
#define PLAYER_ANIMATION_COMPONENT_HPP_

#include "component.hpp"

class PlayerAnimationComponent : public Component
{
public:
  PlayerAnimationComponent();
  ~PlayerAnimationComponent();

  void ReceiveMessage(Message& message){};
  void Update(unsigned int time);
};

#endif
