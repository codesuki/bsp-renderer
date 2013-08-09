#ifndef __RENDERER_H__
#define __RENDERER_H__

glm::mat4 modelmatrix;
glm::mat4 projectionmatrix;
glm::mat4 orthomatrix;

class Renderer
{
public:
  Renderer(void);
  ~Renderer(void);

  void Initialize();

  void SetupFrame();

  void Setup2DRendering();
  void Setup3DRendering();

  void SetupBuffers();

  void RenderFrame();
  void RenderFace();
  void RenderPolygon();
  void RenderPatch();
  void RenderBillboard();

  void AddRenderable(/* type */);
  void AddRenderable(/* type list */);

  void SetupShader();
  void FinishShader();

  // utilities to reduce state changes
  void TexEnvMode(unsigned int texture_unit, unsigned int new_mode);
  void BindTexture(unsigned int texture_unit, unsigned int new_texture);
  void BlendFunc(unsigned int new_blend_func0, unsigned int new_blend_func1);
  void AlphaFunc(unsigned int new_alpha_func, unsigned int new_alpha_val);
  void ActiveTexture(unsigned int new_active_texture);

  void BlendFuncToString();

private:
  unsigned int shader_;
  unsigned int tex_env_mode_[8];
  unsigned int texture_[8];
  unsigned int blend_func_[2];
  unsigned int alpha_func_;
  int active_texture_; // = -1;
  int lightmap_; // = -1;
};

#endif
