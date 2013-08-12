#include "World.h"


World::World(void)
{
}


World::~World(void)
{
}

void World::LoadLevel(std::string name)
{
}

void World::Update()
{
  for (Entity player : players_)
  {
    player.Update();
  }
}
