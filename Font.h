#ifndef __FONT_H__
#define __FONT_H__

#include "util.h"

// in case we support multiple fonts, maybe extract print function.
class Font
{
public:
  Font(void);
  ~Font(void);
  
  int LoadFont(std::string font);
  void PrintString(std::string, glm::vec2& position, glm::vec4& color);

private:
    GLuint texture_;
    GLuint shader_;
    GLuint texture_idx_, projection_idx_, position_idx_, tex_coord_idx_, color_idx_;
};

#endif

