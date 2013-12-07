#include "Entity.h"


Entity::Entity(void) : up_(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)), right_(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)), look_(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f))
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
