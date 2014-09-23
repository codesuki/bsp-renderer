#ifndef PLAYER_PHYSICS_COMPONENT_HPP_
#define PLAYER_PHYSICS_COMPONENT_HPP_

#include "component.hpp"

class Message;

class PlayerPhysicsComponent : public Component
{
public:
  PlayerPhysicsComponent();
  ~PlayerPhysicsComponent();

  void ReceiveMessage(Message& message){};
  void Update(unsigned int time);

  void UpdatePitch();
  void UpdateYaw();
};

#endif
