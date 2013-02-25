#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

namespace TextureLoader
{
  int LoadTexture(std::string filename, q3_shader_stage* shader);
  int LoadLightmap(bsp_lightmap& lightmap, GLuint* texture);
}

#endif /* _TEXTURELOADER_H_ */

