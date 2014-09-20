#include "World.h"


World::World(void)
{
}


World::~World(void)
{
  std::cout << "World deconstructing" << std::endl;
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
