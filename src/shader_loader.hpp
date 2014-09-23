#ifndef SHADER_LOADER_H_
#define SHADER_LOADER_H_

#include <string>

class Shader;

namespace shaderLoader
{
  void Deinitialize();
  int LoadAllShaders();
  int GetShader(std::string name);
  Shader* GetShader(unsigned int id);
  void CompileAllShaders();
  Shader* CreateModelShader(std::string name);
}

#endif

