#include "Q3Shader.h"

#include <string>
#include <GL/GL.h>

Q3Shader::Q3Shader(void)
{
}


Q3Shader::~Q3Shader(void)
{
}

int Q3Shader::ParseShaderStage(const std::string* shaders, int offset, q3_shader_stage* stage)
{
  int i = offset;
  for (; i < shaders->length(); ++i) 
  {
    switch ((*shaders)[i]) 
    {
    case '/':
      if ((*shaders)[i+1] == '/') 
      {
        i = GetNewLinePostion(shaders, i);
        break;
      } 
      break;
    case '{': break;
    case '}': return i;
    case ' ': break;
    case 0x09: break;
    case 0x0A: break;
    case 0x0D: break;
    default:
      int end_pos = GetTokenEndPosition(shaders, i);
      std::string token = shaders->substr(i, end_pos-i);
      i = ++end_pos;

      if (strcasecmp("map", token.c_str()) == 0) 
      {
        end_pos = get_newline_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = end_pos;
        stage->map = token;
        stage->map.erase(0, stage->map.find_first_not_of(' '));
        i = get_newline_pos(shaders, i);
      } 
      else if (strcasecmp("clampmap", token.c_str()) == 0)
      {
        stage->clamp = true;
        end_pos = get_newline_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = end_pos;
        stage->map = token;
        stage->map.erase(0, stage->map.find_first_not_of(' '));
        i = get_newline_pos(shaders, i);   
      } 
      else if (strcasecmp("blendfunc", token.c_str()) == 0) 
      {
        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        if (token.compare("add") == 0 || 
          token.compare("ADD") == 0 || 
          token.compare("Add") == 0 || 
          token.compare("GL_add") == 0) 
        {
          stage->blendfunc[0] = get_blend_func("GL_ONE");
          stage->blendfunc[1] = get_blend_func("GL_ONE");
        } 
        else if (token.compare("filter") == 0)
        {
          stage->blendfunc[0] = get_blend_func("GL_DST_COLOR");
          stage->blendfunc[1] = get_blend_func("GL_ONE");
        } 
        else if (token.compare("blend") == 0) 
        {
          stage->blendfunc[0] = get_blend_func("GL_SRC_ALPHA");
          stage->blendfunc[1] = get_blend_func("GL_ONE_MINUS_SRC_ALPHA");
        } 
        else 
        {
          stage->blendfunc[0] = get_blend_func(token);
          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = end_pos;
          stage->blendfunc[1] = get_blend_func(token);
        }
      } 
      else if (strcasecmp("alphafunc", token.c_str()) == 0) 
      {
        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        stage->alphafunc = get_alpha_func(token);
      } 
      else if (strcasecmp("rgbGen", token.c_str()) == 0) 
      {
        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        if (strcasecmp("identity", token.c_str()) == 0)
        {
          stage->rgbgen = RGBGEN_IDENTITY;
        }
        else if (strcasecmp("vertex", token.c_str()) == 0)
        {
          stage->rgbgen = RGBGEN_VERTEX;
        }
        else if (strcasecmp("wave", token.c_str()) == 0) 
        {
          stage->rgbgen = RGBGEN_WAVE;

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->rgbwave.type = get_wave_func(token); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->rgbwave.base = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->rgbwave.amplitude = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->rgbwave.phase = atof(token.c_str()); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->rgbwave.frequency = atof(token.c_str()); 
        } 
      } 
      else if (strcasecmp("tcmod", token.c_str()) == 0) 
      {
        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        if (strcasecmp("scroll", token.c_str()) == 0) 
        {
          stage->texmods[stage->num_texmods].type = TCMOD_SCROLL;

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].scroll[0] = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].scroll[1] = atof(token.c_str());
        } 
        else if (strcasecmp("scale", token.c_str()) == 0) 
        {
          stage->texmods[stage->num_texmods].type = TCMOD_SCALE;

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].scale[0] = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].scale[1] = atof(token.c_str());
        } 
        /*      else if (strcasecmp("turb", token.c_str()) == 0) 
        {
        stage->texmods[stage->num_texmods++].type = TCMOD_TURB;

        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        stage->texmods[stage->num_texmods].scale[0] = atof(token.c_str());

        end_pos = get_token_end_pos(shaders, i);
        token = shaders->substr(i, end_pos-i);
        i = ++end_pos;
        stage->scale[1] = atof(token.c_str());
        }  */  
        else if (strcasecmp("transform", token.c_str()) == 0) 
        {
          stage->texmods[stage->num_texmods].type = TCMOD_TRANSFORM;           

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].matrix[0][0] = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].matrix[0][1] = atof(token.c_str());    

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].matrix[1][0] = atof(token.c_str());       

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].matrix[1][1] = atof(token.c_str()); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].translate[0] = atof(token.c_str()); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].translate[1] = atof(token.c_str());    

        }   
        else if (strcasecmp("stretch", token.c_str()) == 0) 
        {
          stage->texmods[stage->num_texmods].type = TCMOD_STRETCH;  

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].wave.type = get_wave_func(token); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].wave.base = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].wave.amplitude = atof(token.c_str());

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].wave.phase = atof(token.c_str()); 

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].wave.frequency = atof(token.c_str()); 
        } 
        else if (strcasecmp("rotate", token.c_str()) == 0) 
        {
          stage->texmods[stage->num_texmods].type = TCMOD_ROTATE;

          end_pos = get_token_end_pos(shaders, i);
          token = shaders->substr(i, end_pos-i);
          i = ++end_pos;
          stage->texmods[stage->num_texmods].rotate_speed = atof(token.c_str());
        }                 
        stage->num_texmods++;
      }     
    }
  }
  return i;
}

unsigned int Q3Shader::GetToken(std::string token)
{
}

int Q3Shader::GetTokenEndPosition(const std::string* buffer, int offset)
{
  while (1) 
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

int Q3Shader::GetNewLinePosition(const std::string* buffer, int offset)
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

wavefunc Q3Shader::GetWaveFunc(std::string name)
{
  if (strcasecmp("sin", name.c_str()) == 0)
  {
    return WAVEFUNC_SIN;
  }
  else if (strcasecmp("triangle", name.c_str()) == 0)
  {
    return WAVEFUNC_TRIANGLE;
  }       
  else if (strcasecmp("square", name.c_str()) == 0)
  {
    return WAVEFUNC_SQUARE;
  } 
  else if (strcasecmp("sawtooth", name.c_str()) == 0)
  {
    return WAVEFUNC_SAWTOOTH;
  } 
  else if (strcasecmp("inversesawtooth", name.c_str()) == 0)
  {
    return WAVEFUNC_INVERSESAWTOOTH;
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
