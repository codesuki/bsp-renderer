#ifndef __SHADER_H__
#define __SHADER_H__

#include "util.h"
#include "Q3Shader.h"

class Shader
{
public:
  Shader(const Q3Shader& q3_shader) : q3_shader_(q3_shader), lightmap_stage_(-1) { CompileShader(); };
  ~Shader(void);

  void CompileShader();
  void CompileVertexShader();
  void CompileTesselationShader();
  void CompileFragmentShader();
  void CompileFontShader();

  void SetupTextures();

  GLuint CreateShader(GLenum shader_type, const std::string& shader_file);
  GLuint CreateProgram(const std::vector<GLuint>& shader_list);

  const Q3Shader& q3_shader_;

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

  unsigned int lightmap_stage_;
  unsigned int texture_id_[8];
};

#endif

