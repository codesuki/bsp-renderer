#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

#include <string>
#include "Q3Shader.h"
#include "bsp.h"

namespace textureLoader
{
  int LoadTexture(std::string name, bool clamp);
  int LoadLightmap(bsp_lightmap& lightmap);
  int GetTexture(std::string name, bool clamp);
  int GetLightmap(unsigned int id);
}

#endif /* _TEXTURELOADER_H_ */

