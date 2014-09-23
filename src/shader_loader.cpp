#include "shader_loader.hpp"

#include <fstream>
#include <map>

#include "boost/filesystem.hpp"

#include "logger.hpp"
#include "q3_shader.hpp"
#include "shader.hpp"

namespace shaderLoader
{
namespace {
std::map<std::string, Shader*> shaders_by_name_;
std::vector<Shader*> shaders_;

std::string shaderbuffer_ = "";
unsigned int offset_ = 0;

int LoadShaderFile(std::string file_name, int file_size)
{
  char* buffer = new char[file_size];

  std::ifstream ifs(file_name.c_str(), std::ios::in);
  ifs.read(buffer, file_size);
  ifs.close();

  shaderbuffer_.append(buffer, ifs.gcount());

  if (buffer) delete [] buffer;

  return 0;
}

void LoadAllShaderFiles()
{
  boost::filesystem::path script_path( "./scripts/" );
  boost::filesystem::directory_iterator end_itr;

  for (boost::filesystem::directory_iterator itr( script_path ); itr != end_itr; ++itr)
  {
    if (boost::filesystem::is_directory(itr->status()))
    {
      continue;
    }
    else if (boost::filesystem::is_regular_file(itr->status()))
    {
      LoadShaderFile(itr->path().string(), boost::filesystem::file_size(itr->path()));
    }
  }
}

Q3Shader* ExtractOneShader()
{
  bool is_shader = false;
  std::string name = "";
  Q3Shader* current_shader;

  for (; offset_ < shaderbuffer_.length(); ++offset_) 
  {
    switch (shaderbuffer_[offset_]) 
    {
      case '/':
        if (shaderbuffer_[offset_+1] == '/') 
        {
          // TODO take this out of Q3Shader
          offset_ = Q3Shader::GetNewLinePosition(&shaderbuffer_, offset_);
          break;
        } 
        else 
        {
          name.append(1, '/');
          break;
        }
      case '{': // TODO this is like the worst way ever to handle those shaders..,  change that later
        if (is_shader == true) 
        {
          // sub-shader found
          offset_ = current_shader->ParseShaderStage(&shaderbuffer_, offset_);
        } 
        else 
        {
          // wahrscheinlich doch nur als ptr möglich
          current_shader = new Q3Shader(name);
          is_shader = true;
        }
        break;
      case '}':
        logger::Log(logger::DEBUG, "Read shader %s (%i stages)", current_shader->name_.c_str(), current_shader->stages_.size());
        ++offset_;
        return current_shader;
      case ' ': break;
      case 0x09: break;
      case 0x0A: break;
      case 0x0D: break;
      default:
        name.append(1, shaderbuffer_[offset_]);
    }
  }
  return nullptr;
}

Shader* CreateDefaultShader(std::string name)
{
  logger::Log(logger::DEBUG, "No shader for texture available. Creating default shader and loading texture...");

  Q3Shader* q3_shader = new Q3Shader(name);

  Q3ShaderStage stage = Q3ShaderStage();
  stage.map = name;
  q3_shader->stages_.push_back(stage);

  stage = Q3ShaderStage();
  stage.map = "$lightmap";
  stage.isLightmap = true;
  stage.blendfunc[0] = GL_DST_COLOR;
  stage.blendfunc[1] = GL_ZERO;
  q3_shader->stages_.push_back(stage);	

  Shader* shader = new Shader(*q3_shader);

  shaders_by_name_[name] = shader;

  return shader;
}
}

Shader* CreateModelShader(std::string name)
{
  auto it = shaders_by_name_.find("name");

  if (it == shaders_by_name_.end())
  {
    logger::Log(logger::DEBUG, "Creating model shader...");

    Q3Shader* q3_shader = new Q3Shader(name);

    Q3ShaderStage stage = Q3ShaderStage();
    stage.map = name;
    q3_shader->stages_.push_back(stage);

    Shader* shader = new Shader(*q3_shader);
    shader->CompileShader();

    shaders_by_name_[name] = shader;
  }

  return shaders_by_name_[name];
}


int GetShader(std::string name)
{
  Shader* shader;

  auto it = shaders_by_name_.find(name);
  if (it == shaders_by_name_.end())
  {
    shader = CreateDefaultShader(name);
  }
  else
  {
    shader = it->second;
  }

  shaders_.push_back(shader);

  return 0;
}

Shader* GetShader(unsigned int id)
{
  return shaders_[id];
}

void CompileAllShaders()
{
  for (Shader* shader : shaders_)
  {
    shader->CompileShader();
  }
}

int LoadAllShaders()
{
  // load all shader files into one big string (shaderbuffer_)
  LoadAllShaderFiles();

  // break it up into single shaders
  Q3Shader* q3_shader;
  while (q3_shader = ExtractOneShader(), q3_shader != nullptr)
  {
    Shader* ogl_shader = new Shader(*q3_shader);
    shaders_by_name_[q3_shader->name_] = ogl_shader;
    //delete q3_shader; // not needed after being converted
  }
  shaderbuffer_ = "";
  return 0;
}

void Deinitialize() 
{
  for (auto& shader : shaders_by_name_)
  {
    delete &(shader.second->q3_shader_);
    delete shader.second;
  }
}
}

