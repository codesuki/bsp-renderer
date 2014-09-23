#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <sstream>
#include <vector>

#include <GL/glew.h>

class Q3Shader;

class Shader
{
public:
  Shader(Q3Shader& q3_shader) : q3_shader_(q3_shader),
                                shader_(0),
                                lightmap_stage_(-1),
                                compiled_(false) {};
  ~Shader(void);

  void CompileShader();
  void CompileVertexShader();
  void CompileTesselationShader();
  void CompileFragmentShader();
  void CompileFontShader();

  int SetupTextures();

  GLuint CreateShader(GLenum shader_type, const std::string& shader_file);
  GLuint CreateProgram(const std::vector<GLuint>& shader_list);

  Q3Shader& q3_shader_;

  std::stringstream vertex_shader_;
  std::stringstream tesselation_shader_;
  std::stringstream fragment_shader_;

  unsigned int shader_;

  unsigned int texture_idx_[8]; 
  unsigned int projection_idx_;
  unsigned int model_idx_;
  unsigned int time_idx_;
  unsigned int position_idx_;
  unsigned int tex_coord_idx_;
  unsigned int lm_coord_idx_;
  unsigned int color_idx_;

  int lightmap_stage_;
  unsigned int texture_id_[8];

  bool compiled_;
};

#endif

