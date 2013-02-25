#include "ShaderLoader.h"

#include "boost/filesystem.hpp"

namespace shaderloader
{
  namespace {
    std::string shaders_ = "";

    int LoadShaderFile(std::string file_name, int file_size)
    {
      char* buffer = new char[file_size];

      std::ifstream ifs(file_name.c_str(), std::ios::in);
      ifs.read(buffer, file_size);
      ifs.close();

      shaders_.append(buffer, ifs.gcount());

      SAFE_DELETE_ARRAY(buffer);

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

    std::string ExtractOneShader()
    {
      bool is_shader = false;
      std::string name = "";
      q3_shader* current_shader;

      for (int i = 0; i < g_shaders.length(); ++i) 
      {
        switch (g_shaders[i]) 
        {
        case '/':
          if (g_shaders[i+1] == '/') 
          {
            i = get_newline_pos(&g_shaders, i);
            break;
          } 
          else 
          {
            name.append(1, '/');
            break;
          }
        case '{':
          if (is_shader == true) 
          {
            // sub-shader found
            q3_shader_stage* stage = new q3_shader_stage;
            i = parse_shader_stage(&g_shaders, i, stage);
            current_shader->stages.push_back(stage);
          } 
          else 
          {
            current_shader = new q3_shader();
            current_shader->translucent = false;
            current_shader->name = name;
            m_shaders.insert(std::pair<std::string, q3_shader*>(name, current_shader));
            is_shader = true;
          }

          break;
        case '}':
          Logger::Log(Logger::DEBUG, "Read shader %s (%i stages)", current_shader->name.c_str(), current_shader->stages.size());
          name = "";
          is_shader = false;
          break;
        case ' ':
          break;
        case 0x09: break;
        case 0x0A: break;
        case 0x0D: break;
        default:
          name.append(1, g_shaders[i]);
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

    while (ExtractOneShader())
    {
      Q3Shader shader(name, buffer);
      Shader shader(Q3Shader);
      shaders.push_back(shader);
    }

    return shaders;
  }
}

