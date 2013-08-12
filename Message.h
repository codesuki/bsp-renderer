#ifndef _MESSAGE_H_
#define _MESSAGE_H_

class Message
{
public:
  virtual ~Message(void) {};
};

class MouseMoveMessage : public Message
{
public:
  ~MouseMoveMessage() {};
  MouseMoveMessage(float dx, float dy) : dx_(dx), dy_(dy) {};

  float dx_;
  float dy_;
};

#endif
