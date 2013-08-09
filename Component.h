#pragma once

#include "Message.h"

class Component
{
public:
  virtual ~Component(void) = 0;
  virtual int ReceiveMessage(Message msg) = 0;
};

