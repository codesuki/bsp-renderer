#ifndef __Q3SHADER_H__
#define __Q3SHADER_H__

#include <vector>

#include "util.h"

#define strcasecmp _stricmp

#define MAX_TEXMODS 4

enum class WAVEFUNC 
{
  NONE,
  SIN,
  TRIANGLE,
  SQUARE,
  SAWTOOTH,
  INVERSESAWTOOTH
};

enum class ALPHAFUNC
{
  NONE,
  GREATER,
  LESS,
  GEQUAL
};

enum class RGBGEN
{
  WAVE,
  IDENTITY,
  VERTEX,
  EXACTVERTEX
};

enum class TCMOD
{
  NONE,
  SCROLL,
  SCALE,
  TURB,
  TRANSFORM,
  STRETCH,
  ROTATE
};

struct WaveForm
{ 
  WAVEFUNC type;
  float base;
  float amplitude;
  float phase;
  float frequency; 
};

struct TexMod
{
  TCMOD type;

  WaveForm wave;

  float scale[2];
  float scroll[2];

  float matrix[2][2];
  float translate[2];

  float rotate_speed;

  TexMod()
  {
    type = TCMOD::NONE;
  };
};

struct Q3ShaderStage {
  unsigned int texture;
  bool isLightmap;
  std::string map;
  int blendfunc[2];
  int alphafunc;
  bool depthwrite;
  bool clamp;

  RGBGEN rgbgen;
  WaveForm rgbwave;

  int num_texmods;
  TexMod texmods[MAX_TEXMODS];

  Q3ShaderStage() 
  {
    texture = 0;
    blendfunc[0] = GL_ONE;
    blendfunc[1] = GL_ZERO;
    alphafunc = 0;
    depthwrite = false;
    rgbgen = RGBGEN::IDENTITY;
    num_texmods = 0;
    clamp = false;
  };
};

class Q3Shader
{
public:
  Q3Shader(const std::string name) : name_(name), translucent_(false) {};
  ~Q3Shader(void);

  void ParseShader();
  int ParseShaderStage(const std::string* shaders, int offset);

  int GetAlphaFunc(std::string name);
  int GetBlendFunc(std::string name);
  WAVEFUNC GetWaveFunc(std::string name);

  std::string BlendFuncToString(int blend_func);

  std::string GetToken(const std::string* buffer, int& offset);

  static int GetNewLinePosition(const std::string* buffer, int offset);
  int GetTokenEndPosition(const std::string* buffer, int offset);

  // making this objects instead of pointers we have some additional copying during load time 
  // but don't need to worry about freeing the memory
  std::vector<Q3ShaderStage> stages_;

  bool translucent_;

  std::string name_;
};

#endif
