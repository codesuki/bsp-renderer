#include "Renderer.h"

#include "util.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "World.h"

extern World world;

Renderer::Renderer(void)
{
}


Renderer::~Renderer(void)
{
}

void Renderer::AddRenderables(std::vector<bsp_face*> renderables)
{
  renderables_ = renderables;
}

glm::mat4 Renderer::GetCameraMatrixFromEntity(Entity& entity)
{
  // this is quake3 coordinate system.
  entity.look_ = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  entity.right_ = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
  entity.up_ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

  glm::mat4 matrix(1.0f);

  matrix = glm::rotate(matrix, entity.yaw_, glm::vec3(entity.up_));

  entity.right_ = matrix * entity.right_;
  entity.look_ = matrix * entity.look_;

  matrix = glm::mat4(1.0f);
  matrix = glm::rotate(matrix, entity.pitch_, glm::vec3(entity.right_));

  entity.look_ = matrix * entity.look_;
  entity.up_ = matrix * entity.up_;

  // switch to opengl coordinate system for view matrix calculation
  glm::vec4 right_ogl = quake2ogl * entity.right_;
  glm::vec4 look_ogl = quake2ogl * entity.look_;
  glm::vec4 up_ogl = quake2ogl * entity.up_;
  glm::vec4 position_ogl = quake2ogl * entity.position_;

    //return glm::mat4(look_.x, right_.x, up_.x, 0.0f, 
    //look_.y, right_.y, up_.y, 0.0f,
    //look_.z, right_.z, up_.z, 0.0f, 
    //glm::dot(-position_, look_), glm::dot(-position_, right_), glm::dot(-position_, up_), 1.0f);

  return glm::mat4(right_ogl.x, up_ogl.x, look_ogl.x, 0.0f, 
    right_ogl.y, up_ogl.y, look_ogl.y, 0.0f,
    right_ogl.z, up_ogl.z, look_ogl.z, 0.0f, 
    glm::dot(-position_ogl, right_ogl), glm::dot(-position_ogl, up_ogl), glm::dot(-position_ogl, look_ogl), 1.0f);
}

void Renderer::Initialize()
{
  glEnable(GL_DEPTH_TEST); 
  glDisable(GL_LIGHTING);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

  glViewport(0, 0, screen_width_, screen_height_);

  projectionmatrix_ = glm::perspective(90.0f, (float)screen_width_/(float)screen_height_, 1.0f, 10000.f);
  orthomatrix_ = glm::ortho(0.0f, (float)screen_width_, 0.0f, (float)screen_height_, -1.0f, 1.0f);
}

void Renderer::SetupFrame()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::Setup3DRendering()
{
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  modelmatrix_ = GetCameraMatrixFromEntity(*world.player_);
  modelmatrix_ *= quake2ogl;
  //g_frustum.extract_planes(modelmatrix, projectionmatrix);

  // Graphical commands go here
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO); // WAS ONE before
  //map->render(g_cam.position_, ((float)ticks)/1000.0f);
}

void Renderer::Setup2DRendering()
{
    glDisable(GL_CULL_FACE);
}

void Renderer::RenderFrame(float time)
{
  SetupFrame();
  
  // draw scene
  Setup3DRendering();

  glBindBuffer(GL_ARRAY_BUFFER, bsp_->vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bsp_->iboId);
  glEnableVertexAttribArray(2);

  for (unsigned int i = 0; i < renderables_.size(); ++i) 
  {
    RenderFace(renderables_[i]);
  }

  time_ = time;

  // draw gui and overlays
  Setup2DRendering();

  //font.PrintString("<Q3 BSP RENDERER>", glm::vec2(10.0f, 10.0f), glm::vec4(1.0, 0.0, 0.0, 1.0));

  //  if (delta == 0) delta = 1;	
  //  
  //    std::stringstream fps;
  //    fps << "frametime in ms: " << delta << " fps: " << 1000 / delta;
  //    font.PrintString(fps.str(), glm::vec2(10.0f, (float)screen_height_-20.0f), glm::vec4(1.0, 1.0, 1.0, 1.0));

  // finish frame
  SDL_GL_SwapBuffers();
}

void Renderer::Blend(bool enable)
{
  if (blend_enabled_ != enable)
  {
    blend_enabled_ = enable;
    if (enable) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
  }
}

void Renderer::TexEnvMode(unsigned int texture_unit, unsigned int new_mode) 
{
  if (tex_env_mode_[texture_unit] != new_mode)
  {
    tex_env_mode_[texture_unit] = new_mode;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, new_mode);
  }
}

void Renderer::BindTexture(unsigned int texture_unit, unsigned int new_texture)
{
  if (texture_[texture_unit] != new_texture)
  {
    texture_[texture_unit] = new_texture;
    ActiveTexture(GL_TEXTURE0+texture_unit);
    glBindTexture(GL_TEXTURE_2D, new_texture);
  }
}

void Renderer::BlendFunc(unsigned int new_blend_func0, unsigned int new_blend_func1)
{
  if (blend_func_[0] != new_blend_func0 || blend_func_[1] != new_blend_func1)
  {
    blend_func_[0] = new_blend_func0;
    blend_func_[1] = new_blend_func1;
    glBlendFunc(new_blend_func0, new_blend_func1);
  }
}

void Renderer::AlphaFunc(unsigned int new_alpha_func, unsigned int new_alpha_val)
{
  if (alpha_func_ != new_alpha_func)
  {
    alpha_func_ = new_alpha_func;
    glAlphaFunc(new_alpha_func, new_alpha_val);
  }
}

void Renderer::ActiveTexture(unsigned int new_active_texture)
{ 
  if (active_texture_ != new_active_texture)
  {
    active_texture_ = new_active_texture;
    glActiveTexture(new_active_texture);
  } 
}

void Renderer::FinishShader()
{
  //Blend(false);
}

void Renderer::SetupShader(Shader& shader, int lm_index)
{ 
  // the light map could change even though we have the same shaders
  // check and set new lightmap, leave everthing else the same
  if (&shader == current_shader_)
  {
    if (lm_index == current_lightmap_) return;

    current_lightmap_ = lm_index;
    BindTexture(shader.lightmap_stage_, textureLoader::GetLightmap(lm_index));

    ++num_skipped_shaders_;
    return;
  } 

  // THIS ONLY DISABLES BLENDING BUT WE WANT TO ALWAYS BLEND.. maybe
  //if (current_shader_ != 0)
  //{
  //  FinishShader(*current_shader_);
  //}

  current_shader_ = &shader;

      // JUST ENABLE BLENDING ALL THE TIME AND BLEND NON TRANSLUCENT TEXTURES WITH ONE ZERO
    // only enable blending if stage 0 wants to blend with background
    // shaders can only blend with textures
    //if (i == 0 && stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    //{
      Blend(true);
      BlendFunc(shader.q3_shader_.stages_[0].blendfunc[0], shader.q3_shader_.stages_[0].blendfunc[1]);
    //}

  for (int i = 0; i < 8; ++i)
  {
    // maybe put lightmap directly into stage so we dont need this if
    if (i == shader.lightmap_stage_)
    {
      BindTexture(i, textureLoader::GetLightmap(lm_index));
      --i;
    }
    else
    {
      BindTexture(i, shader.texture_id_[i]);
    } 
  } 
}

void Renderer::RenderFace(bsp_face* face)
{
  const bsp_face &current_face = *face;

  Shader& shader = shaderLoader::GetShader(current_face.texture);

  // does everything in here need to be done every time? move into the conditional below?
  SetupShader(shader, current_face.lm_index);

  if (shader.time_idx_ != -1)
  {
    glUniform1f(shader.time_idx_, time_);   
  } 

  glUniformMatrix4fv(shader.projection_idx_, 1, false, glm::value_ptr(projectionmatrix_));
  glUniformMatrix4fv(shader.model_idx_, 1, false, glm::value_ptr(modelmatrix_));

  if (current_face.type == POLYGON || current_face.type == MESH)
  {
    RenderPolygon(face);
  }
  else if (current_face.type == PATCH)
  {
    RenderPatch(face);
  }
  else if (current_face.type == BILLBOARD)
  {
    RenderBillboard();
  }
}

void Renderer::RenderPolygon(bsp_face* face)
{
  const bsp_face &current_face = *face;
  const int offset = current_face.vertex;

  if (offset >= bsp_->num_vertexes_) return;

  Shader& shader = *current_shader_;

  glVertexAttribPointer(shader.position_idx_, 3, GL_FLOAT, GL_FALSE, sizeof(bsp_vertex), 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)));

  glVertexAttribPointer(shader.tex_coord_idx_, 2, GL_FLOAT, GL_FALSE, sizeof(bsp_vertex), 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)));

  glVertexAttribPointer(shader.lm_coord_idx_, 2, GL_FLOAT, GL_FALSE, sizeof(bsp_vertex), 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)+sizeof(glm::vec2)));

  glVertexAttribPointer(shader.color_idx_, 4, GL_BYTE, GL_FALSE, sizeof(bsp_vertex), 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex) + sizeof(float)*10));    

  glDrawElements(GL_TRIANGLES, 
    current_face.num_meshverts, 
    GL_UNSIGNED_INT, 
    BUFFER_OFFSET(current_face.meshvert * sizeof(bsp_meshvert))); 

}

void Renderer::RenderPatch(bsp_face* face)
{
  /*
  glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
  BUFFER_OFFSET(offset*sizeof(bsp_vertex)));

  glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
  BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)));

  glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
  BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)+sizeof(glm::vec2)));

  glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, stride, 
  BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(float)*10));   

  glPatchParameteri(GL_PATCH_VERTICES, current_face.num_vertices);
  glDrawArrays(GL_PATCHES, 0, current_face.num_vertices);
  */
  //std::vector<bezier*> patches = m_patches[face];

  //for (int i = 0; i < patches.size(); ++i) 
  //{
  //  const bezier* b = patches[i];

  //  glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
  //    BUFFER_OFFSET(b->m_vertex_offset));

  //  glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
  //    BUFFER_OFFSET(b->m_vertex_offset+sizeof(glm::vec3)));

  //  glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
  //    BUFFER_OFFSET(b->m_vertex_offset+sizeof(glm::vec3)+sizeof(glm::vec2)));

  //  glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, stride, 
  //    BUFFER_OFFSET(b->m_vertex_offset+sizeof(float)*10));        

  //  // double work for each bezier, doesnt seem to be needed.. or maybe it does because of vertex colors! then 0 shouldnt be there
  //  //prepare_shader(shader, 0, current_face.lm_index);

  //  unsigned int count[10] = {22,22,22,22,22,22,22,22,22,22};
  //  GLvoid* indices[10];
  //  for (int k = 0; k < 10; k++)
  //  {
  //    indices[k] = (GLvoid*)(b->m_index_offset+sizeof(unsigned int)*k*11*2);
  //  }

  //  glMultiDrawElements(GL_TRIANGLE_STRIP, (const GLsizei*)count, GL_UNSIGNED_INT, (const GLvoid**)indices, 10);
  //}  
}

void Renderer::RenderBillboard()
{
}