#pragma once

#include "util.h"
#include "component.h"
#include "Entity.h"

class PlayerPhysicsComponent : public Component
{
public:
  PlayerPhysicsComponent(Entity& entity);
  ~PlayerPhysicsComponent(void);

  void ReceiveMessage(Message& message){};
  void Update();

private:
  Entity& entity_;
  glm::vec4& position_;
  glm::vec2& orientation_;
  cmd_t& cmds_;

  glm::vec4& up_;
  glm::vec4& right_;
  glm::vec4& look_;
};

