#ifndef TEXTURELOADER_H_
#define TEXTURELOADER_H_

#include <string>

struct bsp_lightmap;

namespace textureLoader
{
  unsigned int skipped_textures();
  void Deinitialize();
  int LoadTexture(std::string name, bool clamp);
  int LoadLightmap(bsp_lightmap& lightmap);
  int GetTexture(std::string name, bool clamp);
  int GetLightmap(unsigned int id);
}

#endif /* _TEXTURELOADER_H_ */

