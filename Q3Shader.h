#ifndef __Q3SHADER_H__
#define __Q3SHADER_H__

#include <vector>

#define strcasecmp _stricmp

#define MAX_TEXMODS 4

enum WaveFunc 
{
  WAVEFUNC_NONE,
  WAVEFUNC_SIN,
  WAVEFUNC_TRIANGLE,
  WAVEFUNC_SQUARE,
  WAVEFUNC_SAWTOOTH,
  WAVEFUNC_INVERSESAWTOOTH
};

enum AlphaFunc
{
  ALPHAFUNC_NONE,
  ALPHAFUNC_GREATER,
  ALPHAFUNC_LESS,
  ALPHAFUNC_GEQUAL
};

enum RgbGen
{
  RGBGEN_WAVE,
  RGBGEN_IDENTITY,
  RGBGEN_VERTEX,
  RGBGEN_EXACTVERTEX
};

enum TcMod
{
  TCMOD_NONE,
  TCMOD_SCROLL,
  TCMOD_SCALE,
  TCMOD_TURB,
  TCMOD_TRANSFORM,
  TCMOD_STRETCH,
  TCMOD_ROTATE
};

struct WaveForm
{ 
  WaveFunc type;
  float base;
  float amplitude;
  float phase;
  float frequency; 
};

struct TexMod
{
  TcMod type;

  WaveForm wave;

  float scale[2];
  float scroll[2];

  float matrix[2][2];
  float translate[2];

  float rotate_speed;

  TexMod()
  {
    type = TCMOD_NONE;
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

  RgbGen rgbgen;
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
    rgbgen = RGBGEN_IDENTITY;
    num_texmods = 0;
    clamp = false;
  };
};

class Q3Shader
{
public:
  Q3Shader(void);
  ~Q3Shader(void);

  void ParseShader();
  void ParseShaderStage();

  int GetAlphaFunc(std::string name);
  int GetBlendFunc(std::string name);
  WaveFunc GetWaveFunc(std::string name);

  std::string BlendFuncToString(int blend_func);

  int GetNewLinePosition();
  int GetTokenEndPosition();

  std::vector<Q3ShaderStage*> stages;

  bool translucent;
  unsigned int shader;
  std::string name;
};

#endif
