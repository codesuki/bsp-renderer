#include "TextureLoader.h"

#include "logger.h"

namespace textureLoader
{
  namespace 
  {
    std::map<std::string, unsigned int> textures_;
    std::vector<unsigned int> lightmaps_;
  }

  int LoadTexture(std::string name, bool clamp) 
  {                                             
    if (name.length() == 0) 
    {
      return -1;
    }
    
    std::string oldname = name;

    if (name.find('.') != std::string::npos)
    {
      name.erase(name.end()-4, name.end());
    }

    std::string filename_tga = name;
    filename_tga.append(".tga");

    std::string filename_jpg = name;
    filename_jpg.append(".jpg");

    SDL_Surface *image;
    image = IMG_Load(filename_jpg.c_str());

    logger::Log(logger::DEBUG, "loading texture: %s\n", filename_jpg.c_str());

    if (!image) 
    {
      logger::Log(logger::DEBUG, "%s", IMG_GetError());

      image = IMG_Load(filename_tga.c_str());
      if (!image) 
      {
        logger::Log(logger::ERROR, "%s", IMG_GetError());
        return -1;
      }
    }

    GLenum texture_format;
    GLenum internal_format;
    GLint num_colors = image->format->BytesPerPixel;

    if (num_colors == 4) // contains an alpha channel
    {
      internal_format = GL_RGBA;
      
      if (image->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
      else
        texture_format = GL_BGRA;
    } 
    else if (num_colors == 3) // no alpha channel
    {
      internal_format = GL_RGB;
      
      if (image->format->Rmask == 0x000000ff)
        texture_format = GL_RGB;
      else
        texture_format = GL_BGR;
    }

    // Have OpenGL generate a texture object handle for us
    unsigned int texture;
    glGenTextures(1, &texture);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, texture);

    if (clamp)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // Set the texture's stretching properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image->w, image->h, 0,
      texture_format, GL_UNSIGNED_BYTE, image->pixels);

    SDL_FreeSurface(image);

    textures_[oldname] = texture;

    return 0;
  }

  int LoadLightmap(bsp_lightmap& lightmap)
  {
    SDL_Surface *image = SDL_CreateRGBSurfaceFrom(static_cast<void*>(&lightmap), 128, 128, 24, 128*3, 0, 0, 0, 0);

    // Have OpenGL generate a texture object handle for us
    unsigned int texture;
    glGenTextures(1, &texture);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the texture's stretching properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image->w, image->h, 0,
      GL_RGB, GL_UNSIGNED_BYTE, image->pixels );

    SDL_FreeSurface(image);

    lightmaps_.push_back(texture);

    return 0;
  }
    unsigned int skipped_textures_ = 0;

    unsigned int skipped_textures()
    {
      return skipped_textures_;
    }

  int GetTexture(std::string texture, bool clamp)
  {
    auto it = textures_.find(texture);
    if (it == textures_.end())
    {
      int ret = LoadTexture(texture, clamp);
      if (ret == -1)
      {
        return -1;
      }
    }
    else
    {
      ++skipped_textures_;
    }
        
    int texture_id = textures_[texture];

    return texture_id;
  }

  int GetLightmap(unsigned int id)
  {
    return lightmaps_[id];
  }

  void Deinitialize()
  {
    for (auto texture : textures_)
    {
      glDeleteTextures(1, &texture.second);
    }

    for (auto texture : lightmaps_)
    {
      glDeleteTextures(1, &texture);
    }
  }
}
