#ifndef MODEL_H_
#define MODEL_H_

#include "Shader.h"

namespace shaderLoader
{
  int LoadAllShaders();
  int GetShader(std::string name);
  Shader& GetShader(unsigned int id);
}

#endif

