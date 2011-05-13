#ifndef _TEXTURE_H_
#define _TEXTURE_H_

class Texture
{
public:
  Texture();
  virtual ~Texture();

  // try to load jpg, if not available tga
  static int LoadTexture(std::string filename, GLuint* texture) 
  {
    if (filename.length() == 0) return -1;

    std::cout << "loading texture: " << filename << std::endl;

    if (filename.find('.') != std::string::npos)
        filename.erase(filename.end()-4, filename.end());
    
    std::string filename_tga = filename;
    filename_tga.append(".tga");
    
    std::string filename_jpg = filename;
    filename_jpg.append(".jpg");

    SDL_Surface *image;
    image = IMG_Load(filename_jpg.c_str());
    if (!image) 
    {
      printf("IMG_Load: %s\n", IMG_GetError());

      image = IMG_Load(filename_tga.c_str());
      if (!image) 
      {
        printf("IMG_Load: %s\n", IMG_GetError());
        return -1;
      }
    }

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
    
    // Have OpenGL generate a texture object handle for us
    glGenTextures(1, texture);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, *texture);
 
    // Set the texture's stretching properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, num_colors, image->w, image->h, 0,
                 texture_format, GL_UNSIGNED_BYTE, image->pixels );

    SAFE_DELETE(image);
    
    return 0;
  }
};

#endif /* _TEXTURE_H_ */

