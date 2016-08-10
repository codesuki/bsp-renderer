#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

class Message;
class Entity;

class Component
{
 protected:
  Entity* entity_;

  bool is_active_ = true;
  
 public:
  
  virtual ~Component(void) {};
  virtual void ReceiveMessage(Message& msg) = 0;
  virtual void Update(unsigned int time) = 0;

  inline void set_entity(Entity* entity)
  {
    entity_ = entity;
  }

  inline void set_active(bool is_active)
  {
    is_active_ = is_active;
  }
};

#endif
