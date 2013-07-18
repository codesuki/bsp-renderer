#ifndef __SHADER_H__
#define __SHADER_H__

#include "util.h"

class Shader
{
public:
  Shader(const Q3Shader& q3_shader) : q3_shader_(q3_shader) {};
  ~Shader(void);

  void CompileShader();
  void CompileVertexShader();
  void CompileTesselationShader();
  void CompileFragmentShader();

  GLuint CreateShader(GLenum shader_type, const std::string& shader_file);
  GLuint CreateProgram(const std::vector<GLuint>& shader_list);

  const Q3Shader& q3_shader_;

  unsigned int shader_;

  unsigned int texture_idx_[8]; 
  unsigned int projection_idx_;
  unsigned int model_idx_;
  unsigned int time_idx_;
  unsigned int position_idx_;
  unsigned int tex_coord_idx_;
  unsigned int lm_coord_idx_;
  unsigned int color_idx_;
};

#endif

