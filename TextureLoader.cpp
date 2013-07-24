#include "TextureLoader.h"

namespace TextureLoader
{
  // try to load jpg, fall back to tga
  int LoadTexture(std::string filename, Q3ShaderStage* shader) 
  {                                             
    if (filename.length() == 0) 
    {
      return -1;
    }

    if (filename.find('.') != std::string::npos)
    {
      filename.erase(filename.end()-4, filename.end());
    }

    std::string filename_tga = filename;
    filename_tga.append(".tga");

    std::string filename_jpg = filename;
    filename_jpg.append(".jpg");

    SDL_Surface *image;
    image = IMG_Load(filename_jpg.c_str());

    printf("loading texture: %s\n", filename_jpg.c_str());

    if (!image) 
    {
      printf("%s\n", IMG_GetError());

      image = IMG_Load(filename_tga.c_str());
      if (!image) 
      {
        printf("%s\n", IMG_GetError());
        return -1;
      }
    }

    GLenum texture_format;
    GLint num_colors = image->format->BytesPerPixel;

    shader->translucent = false;

    if (num_colors == 4) // contains an alpha channel
    {
      if (image->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
      else
        texture_format = GL_BGRA;

      shader->translucent = true;
    } 
    else if (num_colors == 3) // no alpha channel
    {
      if (image->format->Rmask == 0x000000ff)
        texture_format = GL_RGB;
      else
        texture_format = GL_BGR;
    }

    // Have OpenGL generate a texture object handle for us
    glGenTextures(1, &(shader->texture));

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, shader->texture);

    if (shader->clamp)
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
    glTexImage2D(GL_TEXTURE_2D, 0, num_colors, image->w, image->h, 0,
      texture_format, GL_UNSIGNED_BYTE, image->pixels);

    SDL_FreeSurface(image);

    return 0;
  }

  int LoadLightmap(bsp_lightmap& lightmap, GLuint* texture)
  {
    SDL_Surface *image = SDL_CreateRGBSurfaceFrom(static_cast<void*>(&lightmap), 128, 128, 24, 128*3, 0, 0, 0, 0);

    // Have OpenGL generate a texture object handle for us
    glGenTextures(1, texture);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, *texture);

    // Set the texture's stretching properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image->w, image->h, 0,
      GL_RGB, GL_UNSIGNED_BYTE, image->pixels );

    SDL_FreeSurface(image);

    return 0;
  }
}