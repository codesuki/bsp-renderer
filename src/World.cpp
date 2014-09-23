#include "world.hpp"

#include <string>

#include "bsp.hpp"
#include "entity.hpp"

World::World(void)
{
}


World::~World(void)
{
}

void World::LoadLevel(std::string name)
{
  std::string path = "maps/" + name + ".bsp";
  map_ = new Bsp(path);
}

void World::Update(unsigned int time)
{
  for (Entity* player : players_)
  {
    player->Update(time);
  }
}
