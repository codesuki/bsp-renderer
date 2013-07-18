#include "ShaderLoader.h"

#include "boost/filesystem.hpp"

#include "Logger.h"
#include "Q3Shader.h"

namespace shaderloader
{
  namespace {
    std::string shaders_ = "";
    int offset_ = 0;

    int LoadShaderFile(std::string file_name, int file_size)
    {
      char* buffer = new char[file_size];

      std::ifstream ifs(file_name.c_str(), std::ios::in);
      ifs.read(buffer, file_size);
      ifs.close();

      shaders_.append(buffer, ifs.gcount());

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

      for (; offset_ < shaders_.length(); ++offset_) 
      {
        switch (shaders_[offset_]) 
        {
        case '/':
          if (shaders_[offset_+1] == '/') 
          {
            offset_ = Q3Shader::GetNewLinePosition(&shaders_, offset_);
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
            Q3ShaderStage* stage = new Q3ShaderStage();
            offset_ = current_shader->ParseShaderStage(&shaders_, offset_, stage);
            current_shader->stages.push_back(stage);
          } 
          else 
          {
            current_shader = new Q3Shader(name);
            is_shader = true;
          }
          break;
        case '}':
          logger::Log(logger::DEBUG, "Read shader %s (%i stages)", current_shader->name_.c_str(), current_shader->stages.size());
          return current_shader;
        case ' ': break;
        case 0x09: break;
        case 0x0A: break;
        case 0x0D: break;
        default:
          name.append(1, shaders_[offset_]);
        }
      }
    }
  }

  std::vector<Shader> LoadAllShaders()
  {
    // load all shader files into one big string (shaders_)
    LoadAllShaderFiles();

    // break it up into single shaders
    std::vector<Shader> shaders;

    Q3Shader* q3_shader;

    while (q3_shader = ExtractOneShader(), q3_shader != nullptr)
    {
      Shader ogl_shader(q3_shader);
      shaders.push_back(ogl_shader);
    }

    return shaders;
  }
}

