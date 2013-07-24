#include "Renderer.h"

#include "util.h"
#include "bsp.h"

Renderer::Renderer(void)
{
}


Renderer::~Renderer(void)
{
}

void Renderer::Initialize()
{
  glEnable(GL_DEPTH_TEST); 
  glDisable(GL_LIGHTING);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);

#ifndef __USE_SHADERS__
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
#endif

  glViewport(0, 0, WIDTH, HEIGHT);

  projectionmatrix = glm::perspective(90.0f, (float)WIDTH/(float)HEIGHT, 1.0f, 10000.f);
  orthomatrix = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);
}

void Renderer::SetupFrame()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::Setup3DRendering()
{
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  modelmatrix = g_cam.GetMatrix();
  modelmatrix *= quake2ogl;
  g_frustum.extract_planes(modelmatrix, projectionmatrix);

  // Graphical commands go here
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  map->render(g_cam.position_, ((float)ticks)/1000.0f);
}

void Renderer::Setup2DRendering()
{
    glDisable(GL_CULL_FACE);



}

void Renderer::RenderFrame(const glm::vec4& camera_position, float time)
{
  SetupFrame();
  
  // draw scene
  Setup3DRendering();
  // this in world.update()
  get_visible_faces(camera_position);
  std::sort(m_opaque_faces.begin(), m_opaque_faces.end(), faceSort);

  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
  glEnableVertexAttribArray(2);

  for (int i = 0; i < m_opaque_faces.size(); ++i) 
  {
    RenderFace(m_opaque_faces[i]);
  }

  m_time = time;

  // draw gui and overlays
  Setup2DRendering();

  font.PrintString("<Q3 BSP RENDERER>", glm::vec2(10.0f, 10.0f), glm::vec4(1.0, 0.0, 0.0, 1.0));

    if (delta == 0) delta = 1;	
    
      std::stringstream fps;
      fps << "frametime in ms: " << delta << " fps: " << 1000 / delta;
      font.PrintString(fps.str(), glm::vec2(10.0f, (float)HEIGHT-20.0f), glm::vec4(1.0, 1.0, 1.0, 1.0));

  // finish frame
  SDL_GL_SwapBuffers();
}


void Renderer::FinishShader(const Q3Shader& shader)
{
  glDisable(GL_BLEND);
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

void Renderer::SetupShader(Q3Shader& shader, int offset, int lm_index)
{ 
  const bsp_vertex& vertex = m_vertexes[offset];

  // the light map could change even though we have the same shaders
  // check and set new lightmap, leave everthing else the same
  if (&shader == current_shader)
  {
    if (lm_index == current_lm) return;

    current_lm = lm_index;

    for (int i = 0; i < shader.stages.size(); ++i)
    {
      const q3_shader_stage& stage = *(shader.stages[i]);

      if (stage.map.compare("$lightmap") == 0)
      {
        ActiveTexture(GL_TEXTURE0+i);
        BindTexture(i, lightmaps_[lm_index]);
        break;
      }
    }
    ++m_num_skipped_shaders;
    return;
  } 

  if (current_shader != 0)
  {
    FinishShader(*current_shader);
  }

  current_shader = &shader;

  for (int i = 0; i < shader.stages.size(); ++i)
  {
    const q3_shader_stage& stage = *(shader.stages[i]);

    // only enable blending in stage 0 to blend with background
    // shaders can only blend with textures
    if (i == 0 && stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    {
      glEnable(GL_BLEND);
      BlendFunc(stage.blendfunc[0], stage.blendfunc[1]);
    }

    ActiveTexture(GL_TEXTURE0+i);

    // maybe put lightmap directly into stage so we dont need this if
    if (stage.map.compare("$lightmap") == 0)
    {
      BindTexture(i, lightmaps_[lm_index]);
    }
    else
    {
      BindTexture(i, stage.texture);
    } 
  } 
}

void Renderer::RenderFace(bsp_face* face)
{
  const bsp_face &current_face = *face;
  static const int stride = sizeof(bsp_vertex); 
  const int offset = current_face.vertex;

  std::map<std::string, q3_shader*>::iterator it;
  it = m_shaders.find(m_textures[current_face.texture].name);
  q3_shader& shader = *(it->second);

  // does everything in here need to be done every time? move into the conditional below?
  SetupShader(shader, offset, current_face.lm_index);

  if (shader_ != shader.shader)
  {
    glUseProgram(shader.shader);
    shader_ = shader.shader;
  }  

  if (shader.time_idx != -1)
  {
    glUniform1f(shader.time_idx, m_time);   
  } 

  glUniformMatrix4fv(shader.projection_idx, 1, false, glm::value_ptr(projectionmatrix));
  glUniformMatrix4fv(shader.model_idx, 1, false, glm::value_ptr(modelmatrix));

  if (current_face.type == POLYGON || current_face.type == MESH)
  {
    RenderPolygon();
  }
  else if (current_face.type == PATCH)
  {
    RenderPatch();
  }
  else if (current_face.type == BILLBOARD)
  {
    RenderBillboard();
  }
}

void Renderer::RenderPolygon()
{
  if (offset >= m_num_vertexes) return;

  glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)));

  glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)));

  glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(glm::vec3)+sizeof(glm::vec2)));

  glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, sizeof(bsp_vertex), 
    BUFFER_OFFSET(offset*sizeof(bsp_vertex) + sizeof(float)*10));    

  glDrawElements(GL_TRIANGLES, 
    current_face.num_meshverts, 
    GL_UNSIGNED_INT, 
    BUFFER_OFFSET(current_face.meshvert * sizeof(bsp_meshvert))); 

}

void Renderer::RenderPatch()
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
  std::vector<bezier*> patches = m_patches[face];

  for (int i = 0; i < patches.size(); ++i) 
  {
    const bezier* b = patches[i];

    glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
      BUFFER_OFFSET(b->m_vertex_offset));

    glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
      BUFFER_OFFSET(b->m_vertex_offset+sizeof(glm::vec3)));

    glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
      BUFFER_OFFSET(b->m_vertex_offset+sizeof(glm::vec3)+sizeof(glm::vec2)));

    glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, stride, 
      BUFFER_OFFSET(b->m_vertex_offset+sizeof(float)*10));        

    // double work for each bezier, doesnt seem to be needed.. or maybe it does because of vertex colors! then 0 shouldnt be there
    //prepare_shader(shader, 0, current_face.lm_index);

    unsigned int count[10] = {22,22,22,22,22,22,22,22,22,22};
    GLvoid* indices[10];
    for (int k = 0; k < 10; k++)
    {
      indices[k] = (GLvoid*)(b->m_index_offset+sizeof(unsigned int)*k*11*2);
    }

    glMultiDrawElements(GL_TRIANGLE_STRIP, (const GLsizei*)count, GL_UNSIGNED_INT, (const GLvoid**)indices, 10);
  }  
}

void Renderer::RenderBillboard()
{
}