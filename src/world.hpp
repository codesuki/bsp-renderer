#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <vector>

class Bsp;
class Entity;

class World
{
public:
  World(void);
  ~World(void);

  void LoadLevel(std::string name);

  void Update(unsigned int time);

  Bsp* map_;
  Entity* player_;
  Entity* enemy_;
  
  std::vector<Entity*> entities_;
  std::vector<Entity*> players_;
};

#endif

