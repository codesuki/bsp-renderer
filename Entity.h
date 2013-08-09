#pragma once

#include <vector>

#include "Component.h"
#include "Message.h"

struct cmd_t 
{
  char forward_move;
  char right_move;
  char up_move;
};

class Entity
{
public:
  Entity(void);
  ~Entity(void);

  int AddComponent(Component& component) 
  {
  };

  int SendMessage(Message& message)
  {
  }

private:
  std::vector<Component> components_;
  cmd_t cmds_;

};

