#include "Entity.h"


Entity::Entity(void)
{
}


Entity::~Entity(void)
{
}

void Entity::AddComponent(Component* component) 
{
  components_.push_back(component);
}

void Entity::SendMessage(Message& message)
{
}

void Entity::Update(unsigned int time)
{
  for (Component* component : components_)
  {
    component->Update(time);
  }
}
