#pragma once

#include "Message.h"

class Component
{
public:
  virtual ~Component(void) {};
  virtual void ReceiveMessage(Message& msg) = 0;
  virtual void Update(unsigned int time) = 0;
};

