#ifndef __Q3SHADER_H__
#define __Q3SHADER_H__

#include <vector>

#define strcasecmp _stricmp

#define MAX_TEXMODS 4

enum WAVEFUNC 
{
  NONE,
  SIN,
  TRIANGLE,
  SQUARE,
  SAWTOOTH,
  INVERSESAWTOOTH
};

enum ALPHAFUNC
{
  NONE,
  GREATER,
  LESS,
  GEQUAL
};

enum RGBGEN
{
  WAVE,
  IDENTITY,
  VERTEX,
  EXACTVERTEX
};

enum TCMOD
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
  bool translucent;
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
    translucent = false;
    blendfunc[0] = -1;
    blendfunc[1] = -1;
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
  Q3Shader(const std::string name) : name_(name) {};
  ~Q3Shader(void);

  void ParseShader();
  int ParseShaderStage(const std::string* shaders, int offset, Q3ShaderStage* stage);

  int GetAlphaFunc(std::string name);
  int GetBlendFunc(std::string name);
  WAVEFUNC GetWaveFunc(std::string name);

  std::string BlendFuncToString(int blend_func);

  std::string GetToken(const std::string* buffer, int& offset);

  static int GetNewLinePosition(const std::string* buffer, int offset);
  int GetTokenEndPosition(const std::string* buffer, int offset);

  std::vector<Q3ShaderStage*> stages;

  bool translucent;
  unsigned int shader;

  std::string name_;
};

#endif
