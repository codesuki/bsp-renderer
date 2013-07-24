#include "Shader.h"

#include "Q3Shader.h"

Shader::~Shader(void)
{
}

void Shader::CompileShader()
{ 
  if (q3_shader_.stages_.size() == 0)
    return;

  std::string name = q3_shader_.name_;

  std::replace(name.begin(), name.end(), '/', '-');

  std::ofstream ofs;

  std::string fname = "shaders/" + name + ".vert";
  ofs.open(fname.c_str());
  ofs << vertex_shader_.str();
  ofs.close();

  fname = "shaders/" + name + ".frag";
  ofs.open(fname.c_str());
  ofs << fragment_shader_.str();
  ofs.close();

  fname = "shaders/" + name + ".tess";
  ofs.open(fname.c_str());
  ofs << tesselation_shader_.str();
  ofs.close();

  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader_.str()));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader_.str()));

  //shaders.push_back(CreateShader(GL_TESS_EVALUATION_SHADER, tesselation_shader_.str()));

  shader_ = CreateProgram(shaders);

  // maybe this one should be done before linking in createprogram() but cant find any difference. (maybe because only one is used atm)
  glBindFragDataLocation(shader_, 0, "fragColor"); 

  texture_idx_[0] = glGetUniformLocation(shader_, "texture0"); 
  texture_idx_[1] = glGetUniformLocation(shader_, "texture1"); 
  texture_idx_[2] = glGetUniformLocation(shader_, "texture2"); 
  texture_idx_[3] = glGetUniformLocation(shader_, "texture3"); 
  texture_idx_[4] = glGetUniformLocation(shader_, "texture4"); 
  texture_idx_[5] = glGetUniformLocation(shader_, "texture5"); 
  texture_idx_[6] = glGetUniformLocation(shader_, "texture6"); 
  texture_idx_[7] = glGetUniformLocation(shader_, "texture7"); 

  projection_idx_ = glGetUniformLocation(shader_, "inProjectionMatrix");
  model_idx_ = glGetUniformLocation(shader_, "inModelMatrix");
  time_idx_ = glGetUniformLocation(shader_, "inTime");

  position_idx_ = glGetAttribLocation(shader_, "inPosition");
  glEnableVertexAttribArray(position_idx_);
  tex_coord_idx_ = glGetAttribLocation(shader_, "inTexCoord");
  glEnableVertexAttribArray(tex_coord_idx_);
  lm_coord_idx_ = glGetAttribLocation(shader_, "inLmCoord");
  glEnableVertexAttribArray(lm_coord_idx_);
  color_idx_ = glGetAttribLocation(shader_, "inColor");
  glEnableVertexAttribArray(color_idx_);

  glUseProgram(shader_);
  for (int i = 0; i < 8; ++i)
  {
    glUniform1i(texture_idx_[i], i);
  }  
  glUseProgram(0);
}

void Shader::CompileVertexShader()
{
  vertex_shader_ << "#version 410\n" 
    << "uniform mat4 inProjectionMatrix;\n"
    << "uniform mat4 inModelMatrix;\n"
    << "uniform float inTime;\n" 
    << "layout(location = 0) in vec4 inPosition;\n" 
    << "layout(location = 1) in vec2 inTexCoord;\n"
    << "layout(location = 2) in vec2 inLmCoord;\n"
    << "layout(location = 3) in vec4 inColor;\n"
    << "layout(location = 4) out vec2 outLmCoord;\n"
    << "layout(location = 5) out vec4 outColor;\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue; 
    vertex_shader_ << "layout(location = " << 6+i << ") out vec2 outTexCoord" << i << ";\n";
  }

  vertex_shader_ << "void main() {\n"
    << "\toutLmCoord = inLmCoord;\n"
    << "\toutColor = inColor;\n"
    << "\tgl_Position = inProjectionMatrix * inModelMatrix * inPosition;\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue; 

    vertex_shader_ << "\toutTexCoord" << i << " = inTexCoord;\n";
  }  

  vertex_shader_ << "\n\t// texture coordinate modifications\n";
  vertex_shader_ << "\tfloat sinval;\n";
  vertex_shader_ << "\tfloat cosval;\n";
  vertex_shader_ << "\tfloat s;\n";
  vertex_shader_ << "\tfloat t;\n";
  vertex_shader_ << "\tfloat stretch;\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    for (int j = 0; j < MAX_TEXMODS; ++j)
    {
      if (q3_shader_.stages_[i]->texmods[j].type == TCMOD::SCALE)
      {   
        vertex_shader_ << "\toutTexCoord" << i << ".s *= " << q3_shader_.stages_[i]->texmods[j].scale[0] << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t *= " << q3_shader_.stages_[i]->texmods[j].scale[1] << ";\n";
      }

      if (q3_shader_.stages_[i]->texmods[j].type == TCMOD::SCROLL)
      {
        vertex_shader_ << "\toutTexCoord" << i 
          << ".s += inTime * " << q3_shader_.stages_[i]->texmods[j].scroll[0] 
        << " - floor(inTime * " << q3_shader_.stages_[i]->texmods[j].scroll[0] << ")" 
          << ";\n";
        vertex_shader_ << "\toutTexCoord" << i
          << ".t += inTime * " << q3_shader_.stages_[i]->texmods[j].scroll[1]
        << " - floor(inTime * " << q3_shader_.stages_[i]->texmods[j].scroll[1] << ")" 
          << ";\n";
      }

      if (q3_shader_.stages_[i]->texmods[j].type == TCMOD::ROTATE)
      {
        vertex_shader_ << "sinval = sin(radians(inTime * " << q3_shader_.stages_[i]->texmods[j].rotate_speed << "));\n";  
        vertex_shader_ << "cosval = cos(radians(inTime * " << q3_shader_.stages_[i]->texmods[j].rotate_speed << "));\n";  

        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i 
          << ".s = s * cosval + t * -sinval + (0.5 - 0.5 * cosval + 0.5 * sinval)" 
          << ";\n";
        vertex_shader_ << "\toutTexCoord" << i 
          << ".t = s * sinval + t * cosval + (0.5 - 0.5 * sinval - 0.5 * cosval)" 
          << ";\n"; 
      } 

      if (q3_shader_.stages_[i]->texmods[j].type == TCMOD::STRETCH)
      {             
        vertex_shader_ << "sinval = " << q3_shader_.stages_[i]->texmods[j].wave.amplitude << " * " 
          << "sin(" 
          << q3_shader_.stages_[i]->texmods[j].wave.frequency << " * inTime + " << q3_shader_.stages_[i]->texmods[j].wave.phase 
          << ") + " << q3_shader_.stages_[i]->texmods[j].wave.base << ";\n";  

        vertex_shader_ << "stretch = 1.0 / sinval;\n";
        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i << ".s = s * stretch + t * 0 + (0.5 - 0.5 * stretch)" 
          << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t = s * 0 + t * stretch + (0.5 - 0.5 * stretch)" 
          << ";\n";
      }

      if (q3_shader_.stages_[i]->texmods[j].type == TCMOD::TRANSFORM)
      { 
        vertex_shader_ << "s = outTexCoord" << i << ".s;\n";
        vertex_shader_ << "t = outTexCoord" << i << ".t;\n";

        vertex_shader_ << "\toutTexCoord" << i << ".s = s * " << q3_shader_.stages_[i]->texmods[j].matrix[0][0]
        << " + t * " << q3_shader_.stages_[i]->texmods[j].matrix[1][0]
        << " + " << q3_shader_.stages_[i]->texmods[j].translate[0] 
        << ";\n";
        vertex_shader_ << "\toutTexCoord" << i << ".t = s * " << q3_shader_.stages_[i]->texmods[j].matrix[0][1]
        << " + t * " << q3_shader_.stages_[i]->texmods[j].matrix[1][1]
        << " + " << q3_shader_.stages_[i]->texmods[j].translate[1] 
        << ";\n"; 
      }
    }
  }
  vertex_shader_ << "}";
}

void Shader::CompileTesselationShader()
{
  tesselation_shader_ << "#version 410\n"
    << "layout(quads) in;\n" 
    << "uniform mat4 inProjectionMatrix;\n"
    << "uniform mat4 inModelMatrix;\n"
    << "in vec2 outLmCoord[];\n"
    << "in vec4 outColor[];\n"
    << "layout(location = 4) out vec2 toutLmCoord;\n"
    << "layout(location = 5) out vec4 toutColor;\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue;

    tesselation_shader_ << "in vec2 outTexCoord" << i << "[];\n";
  }

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue; 
    tesselation_shader_ << "layout(location = " << 6+i << ") out vec2 toutTexCoord" << i << ";\n";
  }

  tesselation_shader_ << "void main() {\n";

  tesselation_shader_ << "float u = gl_TessCoord.x;\n"
    << "float v = gl_TessCoord.y;\n";

  tesselation_shader_ << "vec2 y; vec2 z; vec4 y4; vec4 z4;";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue; 
    tesselation_shader_ << "\ty = mix(outTexCoord" << i << "[0], outTexCoord" << i << "[6], u);\n";
    tesselation_shader_ << "\tz = mix(outTexCoord" << i << "[2], outTexCoord" << i << "[8], u);\n";
    tesselation_shader_ << "\ttoutTexCoord" << i << " = mix(y, z, v);\n";
  }  
  tesselation_shader_ << "\ty = mix(outLmCoord[0], outLmCoord[6], u);\n";
  tesselation_shader_ << "\tz = mix(outLmCoord[2], outLmCoord[8], u);\n";
  tesselation_shader_ << "toutLmCoord = mix(y, z, v);\n";

  tesselation_shader_ << "\ty4 = mix(outColor[0], outColor[6], u);\n";
  tesselation_shader_ << "\tz4 = mix(outColor[2], outColor[8], u);\n";
  tesselation_shader_ << "toutColor = mix(y4, z4, v);\n";

  tesselation_shader_ << "vec4 a = mix(gl_in[0].gl_Position, gl_in[6].gl_Position, u);\n"
    << "vec4 b = mix(gl_in[2].gl_Position, gl_in[8].gl_Position, u);\n"
    << "vec4 pos = mix(a, b, v);\n"
    << "gl_Position =  pos;\n"
    << "}\n"; 
}

void Shader::CompileFragmentShader()
{
  fragment_shader_ << "#version 410\n"; 

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    fragment_shader_ << "uniform sampler2D texture" << i << "; //" << q3_shader_.stages_[i]->map << "\n";
  }

  fragment_shader_ << "uniform float inTime;\n";
  fragment_shader_ << "layout(location = 4) in vec2 toutLmCoord;\n";
  fragment_shader_ << "layout(location = 5) in vec4 toutColor;\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap")) 
      continue;

    fragment_shader_ << "layout(location = " << 6+i << ") in vec2 toutTexCoord" << i << ";\n";
  }

  fragment_shader_ << "layout(location = 0) out vec4 fragColor;\n";

  fragment_shader_ << "void main() {\n";

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    if (!q3_shader_.stages_[i]->map.compare("$lightmap"))
    {  
      fragment_shader_ << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", toutLmCoord.st);\n";
    }   
    else
    {   
      fragment_shader_ << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", toutTexCoord" << i << ".st);\n";
    }
  }

  fragment_shader_ << "\tfloat sincolor;\n";

  std::string dst;
  std::string src;

  for (int i = 0; i < q3_shader_.stages_.size(); ++i)
  {
    const Q3ShaderStage& stage = *(q3_shader_.stages_[i]);

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
      fragment_shader_ << "\tif (" << src << ".a > 0) discard;\n";
    }
    else if (stage.alphafunc == GL_LESS)
    {
      fragment_shader_ << "\tif (" << src << ".a > 0.5) discard;\n";
    }
    else if (stage.alphafunc == GL_GEQUAL)
    {
      fragment_shader_ << "\tif (" << src << ".a <= 0.5) discard;\n";
    }

    switch (stage.rgbgen)
    {
    case RGBGEN::IDENTITY:
      fragment_shader_ << "\t" << src << " *= ";
      fragment_shader_ << "vec4(1.0, 1.0, 1.0, 1.0);\n";
      break;
      /*case RGBGEN::VERTEX:
      fragment_shader_ << "\t" << src << " *= ";
      fragment_shader_ << "outColor;\n";
      break;*/
    case RGBGEN::WAVE:
      fragment_shader_ << "\tsincolor = clamp(" << q3_shader_.stages_[i]->rgbwave.amplitude << " * " 
        << "sin(" 
        << q3_shader_.stages_[i]->rgbwave.frequency << " * inTime + " << q3_shader_.stages_[i]->rgbwave.phase 
        << ") + " << q3_shader_.stages_[i]->rgbwave.base << ", 0.0, 1.0);\n";

      fragment_shader_ << "\t" << src << " *= vec4(sincolor, sincolor, sincolor, 1.0);\n";
      break;
    default:
      fragment_shader_ << "\t" << src << " *= ";
      fragment_shader_ << "vec4(1.0, 1.0, 1.0, 1.0);\n";
    }

    fragment_shader_ << "\t" << src << " = (" << src << " * ";
    switch (stage.blendfunc[0])
    {
    case GL_ONE:
      fragment_shader_ << "1";
      break;
    case GL_ZERO:
      fragment_shader_ << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader_ << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader_ << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader_ << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader_ << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader_ << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader_ << "(1 - " << dst << ".a)";
      break;
    default:
      std::cout << stage.map << " :: " << stage.blendfunc[0] << std::endl; 
    }

    fragment_shader_ << ") + (" << dst << " * "; 

    switch (stage.blendfunc[1])
    {
    case GL_ONE:
      fragment_shader_ << "1";
      break;
    case GL_ZERO:
      fragment_shader_ << "0";
      break;
    case GL_DST_COLOR:
      fragment_shader_ << dst;
      break;
    case GL_SRC_COLOR:
      fragment_shader_ << src;
      break;
    case GL_DST_ALPHA:
      fragment_shader_ << dst << ".a";
      break;
    case GL_SRC_ALPHA:
      fragment_shader_ << src << ".a";
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      fragment_shader_ << "(1 - " << src << ".a)";
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      fragment_shader_ << "(1 - " << dst << ".a)";
      break;                  
    default:
      std::cout << stage.map << " :: " << stage.blendfunc[1] << std::endl; 
    }

    fragment_shader_ << ");\n"; 
  }   

  fragment_shader_ << "\t" << "fragColor = " << src << ";\n";  
  fragment_shader_ << "}";
}

int Shader::CompileFontShader()
{
    SDL_Surface *image = IMG_Load(font.c_str());

  GLenum texture_format;
  GLint num_colors = image->format->BytesPerPixel;

  if (num_colors == 4) // contains an alpha channel
  {
    if (image->format->Rmask == 0x000000ff)
      texture_format = GL_RGBA;
    else
      texture_format = GL_BGRA;
  } 
  else if (num_colors == 3) // no alpha channel
  {
    if (image->format->Rmask == 0x000000ff)
      texture_format = GL_RGB;
    else
      texture_format = GL_BGR;
  }

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Edit the texture object's image data using the information SDL_Surface gives us
  glTexImage2D(GL_TEXTURE_2D, 0, num_colors, image->w, image->h, 0,
    texture_format, GL_UNSIGNED_BYTE, image->pixels);

  std::stringstream vertex_shader;
  vertex_shader << "#version 130\n" 
    << "uniform mat4 inProjectionMatrix;\n"
    << "in vec2 inPosition;\n" 
    << "in vec2 inTexCoord;\n"
    << "out vec2 outTexCoord;\n";

  vertex_shader << "void main() {\n"
    << "\toutTexCoord = inTexCoord;\n"
    << "\tgl_Position = inProjectionMatrix * vec4(inPosition, 0.0, 1.0);\n";
  vertex_shader << "}";

  std::stringstream fragment_shader;
  fragment_shader << "#version 130\n" 
    << "uniform sampler2D texture;\n"
    << "uniform vec4 color;\n"
    //<< "in vec4 outColor;\n"
    << "in vec2 outTexCoord;\n"
    << "out vec4 fragColor;\n";

  fragment_shader << "void main() {\n";
  fragment_shader << "\tfragColor = texture2D(texture, outTexCoord) * color;\n";
  fragment_shader << "}";

  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader.str()));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader.str()));

  shader_ = CreateProgram(shaders);

  // setup texture input and model/projective matrix in shader
  texture_idx_ = glGetUniformLocation(shader_, "texture"); 
  color_idx_ = glGetUniformLocation(shader_, "color");
  projection_idx_ = glGetUniformLocation(shader_, "inProjectionMatrix");

  position_idx_ = glGetAttribLocation(shader_, "inPosition");
  glEnableVertexAttribArray(position_idx_);
  tex_coord_idx_ = glGetAttribLocation(shader_, "inTexCoord");
  glEnableVertexAttribArray(tex_coord_idx_);

  glUseProgram(shader_);
  glUniform1i(texture_idx_, 0);
  glUseProgram(0);

  SDL_FreeSurface(image);
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