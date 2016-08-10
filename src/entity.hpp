#ifndef ENTITY_H_
#define ENTITY_H_

#include <vector>

#include <glm/glm.hpp>

#include "input.hpp"

class Message;
class Component;
class Model;

class Entity {
  std::vector<Component *> components_;

public:
  Entity(void);
  ~Entity(void);

  void AddComponent(Component *component);
  void SendMessage(Message &message);

  void Update(unsigned int time);

  // share state between components
  cmd_t cmds_;
  glm::vec4 velocity_;
  glm::vec4 position_;
  glm::vec2 orientation_;
  glm::vec4 up_;
  glm::vec4 right_;
  glm::vec4 look_;
  float pitch_;
  float yaw_;

  Model *upper, *lower, *head;
  unsigned int upper_frame, lower_frame;
  bool noclip_;
};

#endif
