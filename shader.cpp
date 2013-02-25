#include "Shader.h"

#include "Q3Shader.h"

Shader::Shader(void)
{
}


Shader::~Shader(void)
{
}

void Shader::CompileShader()
{ 
  if (stages.size() == 0)
    return;

  std::replace(name.begin(), name.end(), '/', '-');

  std::ofstream ofs;

  std::string fname = "shaders/" + name + ".vert";
  ofs.open(fname.c_str());
  ofs << vertex_shader.str();
  ofs.close();

  fname = "shaders/" + name + ".frag";
  ofs.open(fname.c_str());
  ofs << fragment_shader.str();
  ofs.close();

  fname = "shaders/" + name + ".tess";
  ofs.open(fname.c_str());
  ofs << tesselation_shader.str();
  ofs.close();

  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader.str()));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader.str()));

  //shaders.push_back(CreateShader(GL_TESS_EVALUATION_SHADER, tesselation_shader.str()));

  shader_ = CreateProgram(shaders);

  // maybe this one should be done before linking in createprogram() but cant find any difference. (maybe because only one is used atm)
  glBindFragDataLocation(shader, 0, "fragColor"); 

  texture_idx_[0] = glGetUniformLocation(shader, "texture0"); 
  texture_idx_[1] = glGetUniformLocation(shader, "texture1"); 
  texture_idx_[2] = glGetUniformLocation(shader, "texture2"); 
  texture_idx_[3] = glGetUniformLocation(shader, "texture3"); 
  texture_idx_[4] = glGetUniformLocation(shader, "texture4"); 
  texture_idx_[5] = glGetUniformLocation(shader, "texture5"); 
  texture_idx_[6] = glGetUniformLocation(shader, "texture6"); 
  texture_idx_[7] = glGetUniformLocation(shader, "texture7"); 

  projection_idx_ = glGetUniformLocation(shader, "inProjectionMatrix");
  model_idx_ = glGetUniformLocation(shader, "inModelMatrix");
  time_idx_ = glGetUniformLocation(shader, "inTime");

  position_idx_ = glGetAttribLocation(shader, "inPosition");
  glEnableVertexAttribArray(position_idx_);
  tex_coord_idx_ = glGetAttribLocation(shader, "inTexCoord");
  glEnableVertexAttribArray(tex_coord_idx_);
  lm_coord_idx_ = glGetAttribLocation(shader, "inLmCoord");
  glEnableVertexAttribArray(lm_coord_idx_);
  color_idx_ = glGetAttribLocation(shader, "inColor");
  glEnableVertexAttribArray(color_idx_);

  glUseProgram(shader);
  for (int i = 0; i < 8; ++i)
  {
    glUniform1i(texture_idx_[i], i);
  }  
  glUseProgram(0);
}

void Shader::CompileVertexShader()
{
  std::stringstream vertex_shader;
  vertex_shader << "#version 410\n" 
    << "uniform mat4 inProjectionMatrix;\n"
    << "uniform mat4 inModelMatrix;\n"
    << "uniform float inTime;\n" 
    << "layout(location = 0) in vec4 inPosition;\n" 
    << "layout(location = 1) in vec2 inTexCoord;\n"
    << "layout(location = 2) in vec2 inLmCoord;\n"
    << "layout(location = 3) in vec4 inColor;\n"
    << "layout(location = 4) out vec2 outLmCoord;\n"
    << "layout(location = 5) out vec4 outColor;\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue; 
    vertex_shader << "layout(location = " << 6+i << ") out vec2 outTexCoord" << i << ";\n";
  }

  vertex_shader << "void main() {\n"
    << "\toutLmCoord = inLmCoord;\n"
    << "\toutColor = inColor;\n"
    << "\tgl_Position = inProjectionMatrix * inModelMatrix * inPosition;\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue; 

    vertex_shader << "\toutTexCoord" << i << " = inTexCoord;\n";
  }  

  vertex_shader << "\n\t// texture coordinate modifications\n";
  vertex_shader << "\tfloat sinval;\n";
  vertex_shader << "\tfloat cosval;\n";
  vertex_shader << "\tfloat s;\n";
  vertex_shader << "\tfloat t;\n";
  vertex_shader << "\tfloat stretch;\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    for (int j = 0; j < MAX_TEXMODS; ++j)
    {
      if (stages[i]->texmods[j].type == TCMOD_SCALE)
      {   
        vertex_shader << "\toutTexCoord" << i << ".s *= " << stages[i]->texmods[j].scale[0] << ";\n";
        vertex_shader << "\toutTexCoord" << i << ".t *= " << stages[i]->texmods[j].scale[1] << ";\n";
      }

      if (stages[i]->texmods[j].type == TCMOD_SCROLL)
      {
        vertex_shader << "\toutTexCoord" << i 
          << ".s += inTime * " << stages[i]->texmods[j].scroll[0] 
        << " - floor(inTime * " << stages[i]->texmods[j].scroll[0] << ")" 
          << ";\n";
        vertex_shader << "\toutTexCoord" << i
          << ".t += inTime * " << stages[i]->texmods[j].scroll[1]
        << " - floor(inTime * " << stages[i]->texmods[j].scroll[1] << ")" 
          << ";\n";
      }

      if (stages[i]->texmods[j].type == TCMOD_ROTATE)
      {
        vertex_shader << "sinval = sin(radians(inTime * " << stages[i]->texmods[j].rotate_speed << "));\n";  
        vertex_shader << "cosval = cos(radians(inTime * " << stages[i]->texmods[j].rotate_speed << "));\n";  

        vertex_shader << "s = outTexCoord" << i << ".s;\n";
        vertex_shader << "t = outTexCoord" << i << ".t;\n";

        vertex_shader << "\toutTexCoord" << i 
          << ".s = s * cosval + t * -sinval + (0.5 - 0.5 * cosval + 0.5 * sinval)" 
          << ";\n";
        vertex_shader << "\toutTexCoord" << i 
          << ".t = s * sinval + t * cosval + (0.5 - 0.5 * sinval - 0.5 * cosval)" 
          << ";\n"; 
      } 

      if (stages[i]->texmods[j].type == TCMOD_STRETCH)
      {             
        vertex_shader << "sinval = " << stages[i]->texmods[j].wave.amplitude << " * " 
          << "sin(" 
          << stages[i]->texmods[j].wave.frequency << " * inTime + " << stages[i]->texmods[j].wave.phase 
          << ") + " << stages[i]->texmods[j].wave.base << ";\n";  

        vertex_shader << "stretch = 1.0 / sinval;\n";
        vertex_shader << "s = outTexCoord" << i << ".s;\n";
        vertex_shader << "t = outTexCoord" << i << ".t;\n";

        vertex_shader << "\toutTexCoord" << i << ".s = s * stretch + t * 0 + (0.5 - 0.5 * stretch)" 
          << ";\n";
        vertex_shader << "\toutTexCoord" << i << ".t = s * 0 + t * stretch + (0.5 - 0.5 * stretch)" 
          << ";\n";
      }

      if (stages[i]->texmods[j].type == TCMOD_TRANSFORM)
      { 
        vertex_shader << "s = outTexCoord" << i << ".s;\n";
        vertex_shader << "t = outTexCoord" << i << ".t;\n";

        vertex_shader << "\toutTexCoord" << i << ".s = s * " << stages[i]->texmods[j].matrix[0][0]
        << " + t * " << stages[i]->texmods[j].matrix[1][0]
        << " + " << stages[i]->texmods[j].translate[0] 
        << ";\n";
        vertex_shader << "\toutTexCoord" << i << ".t = s * " << stages[i]->texmods[j].matrix[0][1]
        << " + t * " << stages[i]->texmods[j].matrix[1][1]
        << " + " << stages[i]->texmods[j].translate[1] 
        << ";\n"; 
      }
    }
  }
  vertex_shader << "}";
}

void Shader::CompileTesselationShader()
{
  std::stringstream tesselation_shader;

  tesselation_shader << "#version 410\n"
    << "layout(quads) in;\n" 
    << "uniform mat4 inProjectionMatrix;\n"
    << "uniform mat4 inModelMatrix;\n"
    << "in vec2 outLmCoord[];\n"
    << "in vec4 outColor[];\n"
    << "layout(location = 4) out vec2 toutLmCoord;\n"
    << "layout(location = 5) out vec4 toutColor;\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue;

    tesselation_shader << "in vec2 outTexCoord" << i << "[];\n";
  }

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue; 
    tesselation_shader << "layout(location = " << 6+i << ") out vec2 toutTexCoord" << i << ";\n";
  }

  tesselation_shader << "void main() {\n";

  tesselation_shader << "float u = gl_TessCoord.x;\n"
    << "float v = gl_TessCoord.y;\n";

  tesselation_shader << "vec2 y; vec2 z; vec4 y4; vec4 z4;";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue; 
    tesselation_shader << "\ty = mix(outTexCoord" << i << "[0], outTexCoord" << i << "[6], u);\n";
    tesselation_shader << "\tz = mix(outTexCoord" << i << "[2], outTexCoord" << i << "[8], u);\n";
    tesselation_shader << "\ttoutTexCoord" << i << " = mix(y, z, v);\n";
  }  
  tesselation_shader << "\ty = mix(outLmCoord[0], outLmCoord[6], u);\n";
  tesselation_shader << "\tz = mix(outLmCoord[2], outLmCoord[8], u);\n";
  tesselation_shader << "toutLmCoord = mix(y, z, v);\n";

  tesselation_shader << "\ty4 = mix(outColor[0], outColor[6], u);\n";
  tesselation_shader << "\tz4 = mix(outColor[2], outColor[8], u);\n";
  tesselation_shader << "toutColor = mix(y4, z4, v);\n";

  tesselation_shader << "vec4 a = mix(gl_in[0].gl_Position, gl_in[6].gl_Position, u);\n"
    << "vec4 b = mix(gl_in[2].gl_Position, gl_in[8].gl_Position, u);\n"
    << "vec4 pos = mix(a, b, v);\n"
    << "gl_Position =  pos;\n"
    << "}\n"; 
}

void Shader::CompileFragmentShader()
{
  std::stringstream fragment_shader;

  fragment_shader << "#version 410\n"; 

  for (int i = 0; i < stages.size(); ++i)
  {
    fragment_shader << "uniform sampler2D texture" << i << "; //" << stages[i]->map << "\n";
  }

  fragment_shader << "uniform float inTime;\n";
  fragment_shader << "layout(location = 4) in vec2 toutLmCoord;\n";
  fragment_shader << "layout(location = 5) in vec4 toutColor;\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap")) 
      continue;

    fragment_shader << "layout(location = " << 6+i << ") in vec2 toutTexCoord" << i << ";\n";
  }

  fragment_shader << "layout(location = 0) out vec4 fragColor;\n";

  fragment_shader << "void main() {\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap"))
    {  
      fragment_shader << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", toutLmCoord.st);\n";
    }   
    else
    {   
      fragment_shader << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", toutTexCoord" << i << ".st);\n";
    }
  }

  fragment_shader << "\tfloat sincolor;\n";

  std::string dst;
  std::string src;

  for (int i = 0; i < stages.size(); ++i)
  {
    const q3_shader_stage& stage = *(stages[i]);

    std::stringstream helper;
    helper << "color" << i;
    src = helper.str();

    if (i == 0) 
    {
      dst = "vec4(0.0, 0.0, 0.0, 0.0)";
    }
    else
    {
      std::stringstream helper;
      helper << "color" << i-1;
      dst = helper.str();
    }  

    // 0.5 equals 128 when normalized to 0-1 range
    if (stage.alphafunc == GL_GREATER)
    {
      fragment_shader << "\tif (" << src << ".a > 0) discard;\n";
    }
    else if (stage.alphafunc == GL_LESS)
    {
      fragment_shader << "\tif (" << src << ".a > 0.5) discard;\n";
    }
    else if (stage.alphafunc == GL_GEQUAL)
    {
      fragment_shader << "\tif (" << src << ".a <= 0.5) discard;\n";
    }

    switch (stage.rgbgen)
    {
    case RGBGEN_IDENTITY:
      fragment_shader << "\t" << src << " *= ";
      fragment_shader << "vec4(1.0, 1.0, 1.0, 1.0);\n";
      break;
      /*case RGBGEN_VERTEX:
      fragment_shader << "\t" << src << " *= ";
      fragment_shader << "outColor;\n";
      break;*/
    case RGBGEN_WAVE:
      fragment_shader << "\tsincolor = clamp(" << stages[i]->rgbwave.amplitude << " * " 
        << "sin(" 
        << stages[i]->rgbwave.frequency << " * inTime + " << stages[i]->rgbwave.phase 
        << ") + " << stages[i]->rgbwave.base << ", 0.0, 1.0);\n";

      fragment_shader << "\t" << src << " *= vec4(sincolor, sincolor, sincolor, 1.0);\n";
      break;
    default:
      fragment_shader << "\t" << src << " *= ";
      fragment_shader << "vec4(1.0, 1.0, 1.0, 1.0);\n";
    }

    fragment_shader << "\t" << src << " = (" << src << " * ";
    switch (stage.blendfunc[0])
    {
    case GL_ONE:
      fragment_shader << "1";
      break;
    case GL_ZERO:
      fragment_shader << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader << "(1 - " << dst << ".a)";
      break;
    default:
      std::cout << stage.map << " :: " << stage.blendfunc[0] << std::endl; 
    }

    fragment_shader << ") + (" << dst << " * "; 

    switch (stage.blendfunc[1])
    {
    case GL_ONE:
      fragment_shader << "1";
      break;
    case GL_ZERO:
      fragment_shader << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader << "(1 - " << dst << ".a)";
      break;                  
    default:
      std::cout << stage.map << " :: " << stage.blendfunc[1] << std::endl; 
    }

    fragment_shader << ");\n"; 
  }   

  fragment_shader << "\t" << "fragColor = " << src << ";\n";  
  fragment_shader << "}";
}

GLuint Shader::CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
  GLuint shader = glCreateShader(eShaderType);
  const char *strFileData = strShaderFile.c_str();
  glShaderSource(shader, 1, &strFileData, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

    const char *strShaderType = NULL;
    switch(eShaderType)
    {
    case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
    case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
    case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
    }

    fprintf(stdout, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
    delete[] strInfoLog;
  }

  return shader;
}

GLuint Shader::CreateProgram(const std::vector<GLuint> &shaderList)
{
  GLuint program = glCreateProgram();

  for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
    glAttachShader(program, shaderList[iLoop]);

  glLinkProgram(program);

  GLint status;
  glGetProgramiv (program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
    fprintf(stdout, "Linker failure: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  return program;
}