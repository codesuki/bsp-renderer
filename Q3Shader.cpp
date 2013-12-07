#include "Q3Shader.h"

#include <string>

#include "util.h"

Q3Shader::~Q3Shader(void)
{
}

unsigned int Q3Shader::ParseShaderStage(const std::string* shaders, unsigned int offset)
{
  Q3ShaderStage stage;

  unsigned int i = offset;
  for (; i < shaders->length(); ++i) 
  {
    switch ((*shaders)[i]) 
    {
    case '/':
      if ((*shaders)[i+1] == '/') 
      {
        i = GetNewLinePosition(shaders, i);
        break;
      } 
      break;
    case '{': break;
    case '}': 
        stages_.push_back(stage);
        return i;
    case ' ': break;
    case 0x09: break;
    case 0x0A: break;
    case 0x0D: break;
    default:
      std::string token = GetToken(shaders, i);

      if (strcasecmp("map", token.c_str()) == 0) 
      {
        token = GetToken(shaders, i);
        // maybe trim another way, just skip all spaces until next letter after GetToken
        stage.map = token;
        stage.map.erase(0, stage.map.find_first_not_of(' '));

        if (stage.map =="$lightmap")
        {
          stage.isLightmap = true;
        }
      } 
      else if (strcasecmp("clampmap", token.c_str()) == 0)
      {
        stage.clamp = true;
        token = GetToken(shaders, i);
        stage.map = token;
        stage.map.erase(0, stage.map.find_first_not_of(' ')); 
      } 
      else if (strcasecmp("blendfunc", token.c_str()) == 0) 
      {
        token = GetToken(shaders, i);
        if (token.compare("add") == 0 || 
          token.compare("ADD") == 0 || 
          token.compare("Add") == 0 || 
          token.compare("GL_add") == 0) 
        {
          stage.blendfunc[0] = GetBlendFunc("GL_ONE");
          stage.blendfunc[1] = GetBlendFunc("GL_ONE");
        } 
        else if (token.compare("filter") == 0)
        {
          stage.blendfunc[0] = GetBlendFunc("GL_DST_COLOR");
          stage.blendfunc[1] = GetBlendFunc("GL_ONE");
        } 
        else if (token.compare("blend") == 0) 
        {
          stage.blendfunc[0] = GetBlendFunc("GL_SRC_ALPHA");
          stage.blendfunc[1] = GetBlendFunc("GL_ONE_MINUS_SRC_ALPHA");
        } 
        else 
        {
          stage.blendfunc[0] = GetBlendFunc(token);
          token = GetToken(shaders, i);
          stage.blendfunc[1] = GetBlendFunc(token);
        }
      } 
      else if (strcasecmp("alphafunc", token.c_str()) == 0) 
      {
        token = GetToken(shaders, i);
        stage.alphafunc = GetAlphaFunc(token);
      } 
      else if (strcasecmp("rgbGen", token.c_str()) == 0) 
      {
        token = GetToken(shaders, i);
        if (strcasecmp("identity", token.c_str()) == 0)
        {
          stage.rgbgen = RGBGEN::IDENTITY;
        }
        else if (strcasecmp("vertex", token.c_str()) == 0)
        {
          stage.rgbgen = RGBGEN::VERTEX;
        }
        else if (strcasecmp("wave", token.c_str()) == 0) 
        {
          stage.rgbgen = RGBGEN::WAVE;

          token = GetToken(shaders, i);
          stage.rgbwave.type = GetWaveFunc(token); 

          token = GetToken(shaders, i);
          stage.rgbwave.base = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.rgbwave.amplitude = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.rgbwave.phase = atof(token.c_str()); 

          token = GetToken(shaders, i);
          stage.rgbwave.frequency = atof(token.c_str()); 
        } 
      } 
      else if (strcasecmp("tcmod", token.c_str()) == 0) 
      {
        token = GetToken(shaders, i);
        if (strcasecmp("scroll", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::SCROLL;

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].scroll[0] = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].scroll[1] = atof(token.c_str());
        } 
        else if (strcasecmp("scale", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::SCALE;

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].scale[0] = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].scale[1] = atof(token.c_str());
        } 
        else if (strcasecmp("turb", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::TURB;

          token = GetToken(shaders, i);;
          stage.texmods[stage.num_texmods].wave.base = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.amplitude = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.phase = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.frequency = atof(token.c_str());
        }  
        else if (strcasecmp("transform", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::TRANSFORM;           

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].matrix[0][0] = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].matrix[0][1] = atof(token.c_str());    

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].matrix[1][0] = atof(token.c_str());       

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].matrix[1][1] = atof(token.c_str()); 

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].translate[0] = atof(token.c_str()); 

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].translate[1] = atof(token.c_str());    

        }   
        else if (strcasecmp("stretch", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::STRETCH;  

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.type = GetWaveFunc(token); 

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.base = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.amplitude = atof(token.c_str());

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.phase = atof(token.c_str()); 

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].wave.frequency = atof(token.c_str()); 
        } 
        else if (strcasecmp("rotate", token.c_str()) == 0) 
        {
          stage.texmods[stage.num_texmods].type = TCMOD::ROTATE;

          token = GetToken(shaders, i);
          stage.texmods[stage.num_texmods].rotate_speed = atof(token.c_str());
        }                 
        stage.num_texmods++;
      }     
    }
  }
  return i;
}

// make the buffer and offset a class members
std::string Q3Shader::GetToken(const std::string* buffer, unsigned int& offset)
{
  while ((*buffer)[offset] == ' ') ++offset; // skip spaces so that they wont be recognized as token end
  // could call token until its not empty.

  int end_pos = GetTokenEndPosition(buffer, offset);
  std::string token = buffer->substr(offset, end_pos-offset);
  offset = ++end_pos;
  return token;
}

unsigned int Q3Shader::GetTokenEndPosition(const std::string* buffer, unsigned int offset)
{
  while (1) // while buffer[offset] != nullptr
  {
    switch ((*buffer)[offset]) 
    {
    case ' ': 
      return offset;
    case 0x09: 
      return offset;
    case 0x0A: 
      return offset;
    case 0x0D: 
      return offset;
    }
    ++offset;
  }
}

unsigned int Q3Shader::GetNewLinePosition(const std::string* buffer, unsigned int offset)
{
  while (1) 
  {
    switch ((*buffer)[offset]) 
    {
    case 0x0A: 
      return offset;
    case 0x0D: 
      return offset;
    }
    ++offset;
  }
}

WAVEFUNC Q3Shader::GetWaveFunc(std::string name)
{
  if (strcasecmp("sin", name.c_str()) == 0)
  {
    return WAVEFUNC::SIN;
  }
  else if (strcasecmp("triangle", name.c_str()) == 0)
  {
    return WAVEFUNC::TRIANGLE;
  }       
  else if (strcasecmp("square", name.c_str()) == 0)
  {
    return WAVEFUNC::SQUARE;
  } 
  else if (strcasecmp("sawtooth", name.c_str()) == 0)
  {
    return WAVEFUNC::SAWTOOTH;
  } 
  else if (strcasecmp("inversesawtooth", name.c_str()) == 0)
  {
    return WAVEFUNC::INVERSESAWTOOTH;
  }
}

int Q3Shader::GetBlendFunc(std::string name) 
{
  if (strcasecmp("GL_SRC_ALPHA", name.c_str()) == 0) 
  {
    return GL_SRC_ALPHA;
  } 
  else if (strcasecmp("GL_ONE_MINUS_SRC_ALPHA", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_SRC_ALPHA;
  } 
  else if (strcasecmp("GL_ONE", name.c_str()) == 0) 
  {
    return GL_ONE;
  } 
  else if (strcasecmp("GL_ZERO", name.c_str()) == 0) 
  {
    return GL_ZERO;
  } 
  else if (strcasecmp("GL_DST_COLOR", name.c_str()) == 0) 
  {
    return GL_DST_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_DST_COLOR", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_DST_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_SRC_COLOR", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_SRC_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_DST_ALPHA", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_DST_ALPHA;
  } 
  else if (strcasecmp("GL_SRC_COLOR", name.c_str()) == 0) 
  {
    return GL_SRC_COLOR;
  }

  return -1;
}

int Q3Shader::GetAlphaFunc(std::string name) 
{
  if (strcasecmp("GT0", name.c_str()) == 0) 
  {
    return GL_GREATER;
  } 
  else if (strcasecmp("LT128", name.c_str()) == 0) 
  {
    return GL_LESS;
  } 
  else if (strcasecmp("GE128", name.c_str()) == 0) 
  {
    return GL_GEQUAL;
  }
}

std::string Q3Shader::BlendFuncToString(int blend_func)
{
  switch (blend_func)
  {
  case GL_SRC_ALPHA:
    return std::string("GL_SRC_ALPHA");
    break;
  case GL_ONE_MINUS_SRC_ALPHA:
    return std::string("GL_ONE_MINUS_SRC_ALPHA");
    break;
  case GL_ONE:
    return std::string("GL_ONE");
    break;
  case GL_ZERO:
    return std::string("GL_ZERO");
    break;
  case GL_DST_COLOR:
    return std::string("GL_DST_COLOR");
    break;
  case GL_ONE_MINUS_DST_COLOR:
    return std::string("GL_ONE_MINUS_DST_COLOR");
    break;
  case GL_ONE_MINUS_SRC_COLOR:
    return std::string("GL_ONE_MINUS_SRC_COLOR");
    break;
  case GL_ONE_MINUS_DST_ALPHA:
    return std::string("GL_ONE_MINUS_DST_ALPHA");
    break;
  case GL_SRC_COLOR:
    return std::string("GL_SRC_COLOR");
    break;
  default:
    return std::string("Unknown Blend Function");
  }
}
