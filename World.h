#pragma once

#include <vector>

#include "bsp.h"
#include "Entity.h"

class World
{
public:
  World(void);
  ~World(void);

  void LoadLevel(std::string name);

  void Update();

  Bsp* map_;
  Entity* player_;


  std::vector<Entity*> entities_;
  std::vector<Entity*> players_;
};

