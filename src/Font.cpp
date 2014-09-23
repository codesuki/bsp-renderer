#include "font.hpp"

extern glm::mat4 orthomatrix;

Font::Font(void)
{
}


Font::~Font(void)
{
}

int Font::LoadFont(std::string font)
{
  return 0;
}

void Font::PrintString(std::string text, glm::vec2& position, glm::vec4& color)
{
  //std::vector<glm::vec2> vertices;
  //std::vector<glm::vec2> uvcoords;

  //for (int i = 0; i < text.length(); ++i)
  //{
  //  // create big quad from triangles
  //  glm::vec2 upper_left, upper_right, lower_left, lower_right;

  //  upper_left.x = position.x + i * 16.0f;
  //  upper_left.y = position.y + 16.0f;

  //  upper_right.x = position.x + i * 16.0f + 16.0f;
  //  upper_right.y = upper_left.y;

  //  lower_left.x = upper_left.x;
  //  lower_left.y = position.y;

  //  lower_right.x = upper_right.x;
  //  lower_right.y = lower_left.y;

  //  vertices.push_back(lower_left);
  //  vertices.push_back(upper_left);
  //  vertices.push_back(upper_right);

  //  vertices.push_back(lower_right);
  //  vertices.push_back(upper_right);
  //  vertices.push_back(lower_left);

  //  // set texture coordinates to match string
  //  char character = text.at(i);
  //  float col = (character % 16) / 16.0f;
  //  float row = (character / 16) / 16.0f;

  //  upper_left.x = col;
  //  upper_left.y = row; 

  //  upper_right.x = upper_left.x + 1.0f / 16.0f;
  //  upper_right.y = upper_left.y;

  //  lower_left.x = upper_left.x;
  //  lower_left.y = (row + 1.0f / 16.0f);

  //  lower_right.x = lower_left.x + 1.0f / 16.0f;
  //  lower_right.y = (row + 1.0f / 16.0f);

  //  uvcoords.push_back(lower_left);
  //  uvcoords.push_back(upper_left);
  //  uvcoords.push_back(upper_right);

  //  uvcoords.push_back(lower_right);
  //  uvcoords.push_back(upper_right);
  //  uvcoords.push_back(lower_left);
  //}

  //GLuint vboId;
  //GLuint uvboId;

  //glDisableVertexAttribArray(2);

  //glUseProgram(shader_);

  //glGenBuffers(1, &vboId);
  //glBindBuffer(GL_ARRAY_BUFFER, vboId);
  //glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec2), 
  //  vertices.data(), 
  //  GL_STATIC_DRAW);

  //glVertexAttribPointer(position_idx_, 2, GL_FLOAT, GL_FALSE, 0, 
  //  BUFFER_OFFSET(0));

  //glGenBuffers(1, &uvboId);
  //glBindBuffer(GL_ARRAY_BUFFER, uvboId);
  //glBufferData(GL_ARRAY_BUFFER, uvcoords.size()*sizeof(glm::vec2), 
  //  uvcoords.data(), 
  //  GL_STATIC_DRAW);

  //glVertexAttribPointer(tex_coord_idx_, 2, GL_FLOAT, GL_FALSE, 0, 
  //  BUFFER_OFFSET(0));

  //glEnable(GL_TEXTURE_2D);
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, texture_);

  //glUniformMatrix4fv(projection_idx_, 1, false, glm::value_ptr(orthomatrix));
  //glUniform4fv(color_idx_, 1, glm::value_ptr(color));
  //
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glDrawArrays(GL_TRIANGLES, 0, vertices.size()); 
  //glDisable(GL_BLEND);

  //glUseProgram(0);
}
