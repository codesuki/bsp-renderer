#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "util.h"
#include "Shader.h"
#include "bsp.h"
#include "Entity.h"

#define WIDTH 1280
#define HEIGHT 720

static const glm::mat4 quake2ogl(0.0f, 0.0f, -1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

class Renderer
{
public:
  void RenderModel();

  Renderer(void);
  ~Renderer(void);

  void Initialize();

  void SetupFrame();

  void Setup2DRendering();
  void Setup3DRendering();

  void SetupBuffers();

  void RenderFrame(float time);
  void RenderFace(bsp_face* face);
  void RenderPolygon(bsp_face* face);
  void RenderPatch(bsp_face* face);
  void RenderBillboard();

  //void AddRenderable(/* type */);
  void AddRenderables(std::vector<bsp_face*> renderables);

  void SetupShader(Shader* shader, int lm_index);
  void FinishShader();

  // utilities to reduce state changes
  void Blend(bool enable);
  void TexEnvMode(unsigned int texture_unit, unsigned int new_mode);
  void BindTexture(unsigned int texture_unit, unsigned int new_texture);
  void BlendFunc(unsigned int new_blend_func0, unsigned int new_blend_func1);
  void AlphaFunc(unsigned int new_alpha_func, unsigned int new_alpha_val);
  void ActiveTexture(unsigned int new_active_texture);

  glm::mat4 GetCameraMatrixFromEntity(Entity& entity);

private:
  bool blend_enabled_;
  unsigned int tex_env_mode_[8];
  unsigned int texture_[8];
  unsigned int blend_func_[2];
  unsigned int alpha_func_;
  int active_texture_; // = -1;

  Shader* current_shader_;
  int current_lightmap_; // = -1;

  unsigned int num_skipped_shaders_;

  int screen_width_;
  int screen_height_;

  glm::mat4 modelmatrix_;
  glm::mat4 projectionmatrix_;
  glm::mat4 orthomatrix_;

  float time_;
  
  Bsp* bsp_;

  std::vector<bsp_face*> renderables_;
};

#endif
