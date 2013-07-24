#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

#include <string>
#include "Q3Shader.h"
#include "bsp.h"

namespace textureLoader
{
  int LoadTexture(std::string filename, Q3ShaderStage* shader);
  int LoadLightmap(bsp_lightmap& lightmap, GLuint* texture);
}

#endif /* _TEXTURELOADER_H_ */

