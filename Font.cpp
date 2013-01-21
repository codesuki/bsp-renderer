#include "Font.h"

#include "util.h"

Font::Font(void)
{
}


Font::~Font(void)
{
}

int Font::LoadFont(std::string font)
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

  return 0;
}

extern glm::mat4 orthomatrix;

void Font::PrintString(std::string text, glm::vec2& position, glm::vec4& color)
{
  std::vector<glm::vec2> vertices;
  std::vector<glm::vec2> uvcoords;

  for (int i = 0; i < text.length(); ++i)
  {
    // create big quad from triangles
    glm::vec2 upper_left, upper_right, lower_left, lower_right;

    upper_left.x = position.x + i * 16.0f;
    upper_left.y = position.y + 16.0f;

    upper_right.x = position.x + i * 16.0f + 16.0f;
    upper_right.y = upper_left.y;

    lower_left.x = upper_left.x;
    lower_left.y = position.y;

    lower_right.x = upper_right.x;
    lower_right.y = lower_left.y;

    vertices.push_back(lower_left);
    vertices.push_back(upper_left);
    vertices.push_back(upper_right);

    vertices.push_back(lower_right);
    vertices.push_back(upper_right);
    vertices.push_back(lower_left);

    // set texture coordinates to match string
    char character = text.at(i);
    float col = (character % 16) / 16.0f;
    float row = (character / 16) / 16.0f;

    upper_left.x = col;
    upper_left.y = row; 

    upper_right.x = upper_left.x + 1.0f / 16.0f;
    upper_right.y = upper_left.y;

    lower_left.x = upper_left.x;
    lower_left.y = (row + 1.0f / 16.0f);

    lower_right.x = lower_left.x + 1.0f / 16.0f;
    lower_right.y = (row + 1.0f / 16.0f);

    uvcoords.push_back(lower_left);
    uvcoords.push_back(upper_left);
    uvcoords.push_back(upper_right);

    uvcoords.push_back(lower_right);
    uvcoords.push_back(upper_right);
    uvcoords.push_back(lower_left);
  }

  GLuint vboId;
  GLuint uvboId;

  glDisableVertexAttribArray(2);

  glUseProgram(shader_);

  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec2), 
    vertices.data(), 
    GL_STATIC_DRAW);

  glVertexAttribPointer(position_idx_, 2, GL_FLOAT, GL_FALSE, 0, 
    BUFFER_OFFSET(0));

  glGenBuffers(1, &uvboId);
  glBindBuffer(GL_ARRAY_BUFFER, uvboId);
  glBufferData(GL_ARRAY_BUFFER, uvcoords.size()*sizeof(glm::vec2), 
    uvcoords.data(), 
    GL_STATIC_DRAW);

  glVertexAttribPointer(tex_coord_idx_, 2, GL_FLOAT, GL_FALSE, 0, 
    BUFFER_OFFSET(0));

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glUniformMatrix4fv(projection_idx_, 1, false, glm::value_ptr(orthomatrix));
  glUniform4fv(color_idx_, 1, glm::value_ptr(color));
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size()); 
  glDisable(GL_BLEND);

  glUseProgram(0);
}
