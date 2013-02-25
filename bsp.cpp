#include "bsp.h"

#include "util.h"
#include "bezier.h"
#include "frustum.h"
#include "Logger.h"

bsp::bsp(void)
{

}

bsp::~bsp(void)
{
  SAFE_DELETE_ARRAY(entities_->ents);
  SAFE_DELETE(entities_);
  SAFE_DELETE_ARRAY(textures_);
  SAFE_DELETE_ARRAY(planes_);
  SAFE_DELETE_ARRAY(nodes_);
  SAFE_DELETE_ARRAY(planes_);
  SAFE_DELETE_ARRAY(leafs_);
  SAFE_DELETE_ARRAY(leaffaces_);
  SAFE_DELETE_ARRAY(leafbrushes_);
  SAFE_DELETE_ARRAY(models_);
  SAFE_DELETE_ARRAY(brushes_);
  SAFE_DELETE_ARRAY(brushsides_);
  SAFE_DELETE_ARRAY(vertexes_);
  SAFE_DELETE_ARRAY(meshverts_);
  SAFE_DELETE_ARRAY(effects_);
  SAFE_DELETE_ARRAY(faces_);
  SAFE_DELETE_ARRAY(lightmaps_);
  SAFE_DELETE_ARRAY(lightvols_);
  SAFE_DELETE_ARRAY(visdata_->vecs);
  SAFE_DELETE(visdata_);

  {
    std::map<bsp_face*, std::vector<bezier*> >::iterator it;
    for (it = patches_.begin(); it != patches_.end(); ++it)
      for (int i = 0; i < (*it).second.size(); ++i)
        SAFE_DELETE(((*it).second)[i]);
  }

  {
    std::map<std::string, q3_shader*>::iterator it;
    for (it = shaders_.begin(); it != shaders_.end(); ++it) 
    {
      for (int i = 0; i < (*it).second->stages.size(); ++i) 
      {
        // TODO free textures
        SAFE_DELETE((*it).second->stages[i]);
      }
      SAFE_DELETE((*it).second);
    }
  }
}

bsp::bsp(std::string filename)
{
  std::ifstream fin(filename.c_str(), std::ios::binary);

  fin.read((char*)&header_, sizeof(bsp_header));

  entities_ = new bsp_entities;
  entities_->ents = new char[header_.direntries[LUMP_ENTITIES].length];

  num_textures_ = header_.direntries[LUMP_TEXTURES].length/sizeof(bsp_texture);
  num_planes_ = header_.direntries[LUMP_PLANES].length/sizeof(bsp_plane);
  num_nodes_ = header_.direntries[LUMP_NODES].length/sizeof(bsp_node);
  num_planes_ = header_.direntries[LUMP_PLANES].length/sizeof(bsp_plane);
  num_leafs_ = header_.direntries[LUMP_LEAFS].length/sizeof(bsp_leaf);
  num_leaffaces_ = header_.direntries[LUMP_LEAFFACES].length/sizeof(bsp_leafface);
  num_leafbrushes_ = header_.direntries[LUMP_LEAFBRUSHES].length/sizeof(bsp_leafbrush);
  num_models_ = header_.direntries[LUMP_MODELS].length/sizeof(bsp_model);
  num_brushes_ = header_.direntries[LUMP_BRUSHES].length/sizeof(bsp_brush);
  num_brushsides_ = header_.direntries[LUMP_BRUSHSIDES].length/sizeof(bsp_brushside);
  num_vertexes_ = header_.direntries[LUMP_VERTEXES].length/sizeof(bsp_vertex);
  num_meshverts_ = header_.direntries[LUMP_MESHVERTS].length/sizeof(bsp_meshvert);
  num_effects_ = header_.direntries[LUMP_EFFECTS].length/sizeof(bsp_effect);
  num_faces_ = header_.direntries[LUMP_FACES].length/sizeof(bsp_face);
  num_lightmaps_ = header_.direntries[LUMP_LIGHTMAPS].length/sizeof(bsp_lightmap);
  num_lightvols_ = header_.direntries[LUMP_LIGHTVOLS].length/sizeof(bsp_lightvol);

  textures_ = new bsp_texture[num_textures_];
  nodes_ = new bsp_node[num_nodes_];
  planes_ = new bsp_plane[num_planes_];
  leafs_ = new bsp_leaf[num_leafs_];
  leaffaces_ = new bsp_leafface[num_leaffaces_];
  leafbrushes_ = new bsp_leafbrush[num_leafbrushes_];
  models_ = new bsp_model[num_models_];
  brushes_ = new bsp_brush[num_brushes_];
  brushsides_ = new bsp_brushside[num_brushsides_];
  vertexes_ = new bsp_vertex[num_vertexes_];
  meshverts_ = new bsp_meshvert[num_meshverts_];
  effects_ = new bsp_effect[num_effects_];
  faces_ = new bsp_face[num_faces_];
  lightmaps_ = new bsp_lightmap[num_lightmaps_];
  lightvols_ = new bsp_lightvol[num_lightvols_];
  visdata_ = new bsp_visdata;

  fin.seekg(header_.direntries[LUMP_ENTITIES].offset);
  fin.read((char*)entities_->ents, header_.direntries[LUMP_ENTITIES].length);

  fin.seekg(header_.direntries[LUMP_TEXTURES].offset);
  fin.read((char*)textures_, header_.direntries[LUMP_TEXTURES].length);

  fin.seekg(header_.direntries[LUMP_PLANES].offset);
  fin.read((char*)planes_, header_.direntries[LUMP_PLANES].length);

  fin.seekg(header_.direntries[LUMP_NODES].offset);
  fin.read((char*)nodes_, header_.direntries[LUMP_NODES].length);

  fin.seekg(header_.direntries[LUMP_LEAFS].offset);
  fin.read((char*)leafs_, header_.direntries[LUMP_LEAFS].length);

  fin.seekg(header_.direntries[LUMP_LEAFFACES].offset);
  fin.read((char*)leaffaces_, header_.direntries[LUMP_LEAFFACES].length);

  fin.seekg(header_.direntries[LUMP_LEAFBRUSHES].offset);
  fin.read((char*)leafbrushes_, header_.direntries[LUMP_LEAFBRUSHES].length);

  fin.seekg(header_.direntries[LUMP_MODELS].offset);
  fin.read((char*)models_, header_.direntries[LUMP_MODELS].length);

  fin.seekg(header_.direntries[LUMP_BRUSHES].offset);
  fin.read((char*)brushes_, header_.direntries[LUMP_BRUSHES].length);

  fin.seekg(header_.direntries[LUMP_BRUSHSIDES].offset);
  fin.read((char*)brushsides_, header_.direntries[LUMP_BRUSHSIDES].length);

  fin.seekg(header_.direntries[LUMP_VERTEXES].offset);
  fin.read((char*)vertexes_, header_.direntries[LUMP_VERTEXES].length);

  fin.seekg(header_.direntries[LUMP_MESHVERTS].offset);
  fin.read((char*)meshverts_, header_.direntries[LUMP_MESHVERTS].length);

  fin.seekg(header_.direntries[LUMP_EFFECTS].offset);
  fin.read((char*)effects_, header_.direntries[LUMP_EFFECTS].length);

  fin.seekg(header_.direntries[LUMP_FACES].offset);
  fin.read((char*)faces_, header_.direntries[LUMP_FACES].length);

  fin.seekg(header_.direntries[LUMP_LIGHTMAPS].offset);
  fin.read((char*)lightmaps_, header_.direntries[LUMP_LIGHTMAPS].length);

  fin.seekg(header_.direntries[LUMP_LIGHTVOLS].offset);
  fin.read((char*)lightvols_, header_.direntries[LUMP_LIGHTVOLS].length);

  fin.seekg(header_.direntries[LUMP_VISDATA].offset);
  fin.read((char*)visdata_, 2*sizeof(int));
  visdata_->vecs = new unsigned char[visdata_->num_vecs*visdata_->size_vecs];
  fin.read((char*)visdata_->vecs, sizeof(unsigned char)*visdata_->num_vecs*visdata_->size_vecs);

  fin.close();

  // prepare patch data + tesselate
  for (int i = 0; i < num_faces_; ++i) 
  {
    if (faces_[i].type == PATCH) 
    {
      bsp_face* face = &(faces_[i]);

      int width         = faces_[i].size[0];
      int height        = faces_[i].size[1];
      int widthCount    = (width - 1) / 2;
      int heightCount   = (height - 1) / 2;

      patches_[face].resize(widthCount*heightCount);
      for (int j = 0; j < widthCount*heightCount; ++j) 
      {
        patches_[face][j] = new bezier();
      }

      for (int y = 0; y < heightCount; y++) 
      {
        for (int x = 0; x < widthCount; x++) 
        {
          for (int row = 0; row < 3; row++) 
          {
            for (int col = 0; col < 3; col++) 
            {
              patches_[face][y * widthCount + x]->m_control_vertexes[row * 3 + col] =
                vertexes_[faces_[i].vertex +(y * 2 * width + x * 2)+row * width + col];
            }
          }
          patches_[face][y * widthCount + x]->tessellate(10);
        }
      }
    }
  }

  Logger::Log(Logger::DEBUG, "Loading shader files");

  load_shaders();

  Logger::Log(Logger::DEBUG, "Checking if all needed textures are loaded");

  for (int i = 0; i < num_textures_; ++i) 
  {
    std::map<std::string, q3_shader*>::iterator it;
    it = shaders_.find(textures_[i].name);

    Logger::Log(Logger::DEBUG, "Checking texture: %s, %i, %i", textures_[i].name, textures_[i].flags, textures_[i].contents);

    if (it == shaders_.end()) 
    {
      Logger::Log(Logger::DEBUG, "No shader for texture available. Creating shader and loading texture...");

      q3_shader* shader = new q3_shader();
      shader->translucent = false;
      shader->name = textures_[i].name;

      q3_shader_stage* stage = new q3_shader_stage;
      stage->blendfunc[0] = GL_ONE;
      stage->blendfunc[1] = GL_ZERO;
      int res = Texture::LoadTexture(textures_[i].name, (stage)); 
      stage->map = textures_[i].name;
      if (res != 0) {
        stage->map = "noshader.tga";
        stage->texture = NULL;
      }
      shader->stages.push_back(stage);

      if (stage->translucent)
        shader->translucent = true;

      stage = new q3_shader_stage;
      stage->map = "$lightmap";
      stage->blendfunc[0] = GL_DST_COLOR;
      stage->blendfunc[1] = GL_ZERO;
      shader->stages.push_back(stage);		 

      shaders_.insert(std::pair<std::string, q3_shader*>(textures_[i].name, shader));
    } 
    else 
    {
      q3_shader *shader = it->second;
      shader->translucent = false;
      Logger::Log(Logger::DEBUG, "Shader (%i stages) for texture found. Loading texture...", shader->stages.size());

      for (int i = 0; i < shader->stages.size(); ++i) 
      {
        // some shaders don't have blendfunc values so set a default.
        // maybe remove in future and just dont process.
        if (i == 0)
        {
          if (shader->stages[i]->blendfunc[0] == -1)
          {
            shader->stages[i]->blendfunc[0] = GL_ONE;
            shader->stages[i]->blendfunc[1] = GL_ZERO;
          }
        } 
        int res = 0;
        shader->stages[i]->texture = NULL;
        if (shader->stages[i]->map.compare("$whiteimage") == 0) 
        {
          continue;
        } 
        else if (shader->stages[i]->map.compare("$lightmap") == 0) 
        {
          continue;
        } 
        else 
        {
          res = Texture::LoadTexture(shader->stages[i]->map.c_str(), (shader->stages[i]));
        }
        if (res != 0) {
          shader->stages[i]->map = "noshader.tga";
        }

        if (shader->stages[i]->translucent)
          shader->translucent = true;
      }
    }
  }
  Logger::Log(Logger::DEBUG, "Finished loading all needed textures");    

  // remove lightmap stage for faces without lightmap.
  for (int i = 0; i < num_faces_; ++i)
  {
    if (faces_[i].lm_index >= 0)
    {
      continue;
    }
    std::map<std::string, q3_shader*>::iterator it;
    it = shaders_.find(textures_[faces_[i].texture].name);
    q3_shader& shader = *(it->second);
    for (int j = 0; j < shader.stages.size(); ++j)
    {
      if (shader.stages[j]->map.compare("$lightmap") == 0)
      {
        shader.stages.pop_back();
        Logger::Log(Logger::DEBUG, "Removed lightmap from shader %s", textures_[faces_[i].texture].name);
      }
    }
  } 

  Logger::Log(Logger::DEBUG, "Loading lightmaps...");
  load_lightmaps();
  Logger::Log(Logger::DEBUG, "Finished loading lightmaps");


  {
    std::map<std::string, q3_shader*>::iterator it;

    for (int i = 0; i < num_textures_; ++i) 
    {
      std::map<std::string, q3_shader*>::iterator it;
      it = shaders_.find(textures_[i].name);

      //std::cout << "processing: " << it->first << std::endl;
      it->second->compile();
    }
  }

  // calculate correct buffer size
  // all vertexes + all tesselated beziers
  int num_bezier_vertexes = 0;
  int num_bezier_indexes = 0;

  std::map<bsp_face*, std::vector<bezier*> >::iterator it;  

  for (it = patches_.begin(); it != patches_.end(); ++it)
  {
    num_bezier_vertexes += 11*11*it->second.size();
    num_bezier_indexes += 10*11*2*it->second.size();
  }

  // generate a new VBO and get the associated ID
  glGenBuffers(1, &vboId);

  // bind VBO in order to use
  glBindBuffer(GL_ARRAY_BUFFER, vboId);

  // upload data to VBO
  glBufferData(GL_ARRAY_BUFFER, 
    header_.direntries[LUMP_VERTEXES].length + num_bezier_vertexes*sizeof(bsp_vertex), 
    NULL, 
    GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, header_.direntries[LUMP_VERTEXES].length, vertexes_);

  glGenBuffers(1, &iboId);

  // bind VBO in order to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // upload data to VBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
    header_.direntries[LUMP_MESHVERTS].length + num_bezier_indexes*sizeof(unsigned int), 
    NULL, 
    GL_STATIC_DRAW); 

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, header_.direntries[LUMP_MESHVERTS].length, meshverts_); 

  int offset_verts = 0;
  int offset_idx = 0;
  for (it = patches_.begin(); it != patches_.end(); ++it)
  {
    for (int j = 0; j < it->second.size(); ++j)
    {
      glBufferSubData(GL_ARRAY_BUFFER,
        header_.direntries[LUMP_VERTEXES].length + offset_verts, 
        11*11*sizeof(bsp_vertex), 
        it->second[j]->m_vertexes);

      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
        header_.direntries[LUMP_MESHVERTS].length + offset_idx,
        10*11*2*sizeof(unsigned int), 
        it->second[j]->m_indexes); 

      it->second[j]->m_vertex_offset = header_.direntries[LUMP_VERTEXES].length + offset_verts;
      it->second[j]->m_index_offset = header_.direntries[LUMP_MESHVERTS].length + offset_idx;

      offset_verts += sizeof(bsp_vertex)*11*11;
      offset_idx += sizeof(unsigned int)*10*11*2;    
    }
  } 
}

void bsp::load_lightmaps()
{
  lightmaps_ = new GLuint[num_lightmaps_];
  for (int i = 0; i < num_lightmaps_; ++i)
  {
    Texture::loadLightmap(lightmaps_[i], &(lightmaps_[i]));  
  }
}

int bsp::find_leaf(const glm::vec4& camera_position)
{
  int index = 0;

  while (index >= 0) 
  {
    const bsp_node& node = nodes_[index];
    const bsp_plane& plane = planes_[node.plane];

    // TODO: multiplicate the plane with our transformation matrices (inverse and transpose needed for plane transformation)
    // TODO: maybe do this during load time to enhance framerate! 

    // Distance from point to a plane

    glm::vec4 pos = camera_position;

    /*  if (plane.type < 3) // type < 3 -> axial plane
    {
    const float distance = pos[plane->type] - plane.distance;
    }
    else
    */  
    const float distance = glm::dot(plane.normal, glm::vec3(pos)) - plane.distance;

    if (distance >= 0) 
    {
      index = node.front;
    } 
    else 
    {
      index = node.back;
    }
  }

  return -index - 1;
}

bool bsp::is_cluster_visible(int cluster, int test_cluster)
{
  if ((visdata_->vecs == NULL) || (cluster < 0)) return true;

  int i = (cluster * visdata_->size_vecs) + (test_cluster >> 3);
  unsigned char visSet = visdata_->vecs[i];

  if (!(visSet & (1 << (test_cluster & 7)))) return false;
  return true;
}

void bsp::get_visible_faces(const glm::vec4& camera_position)
{
  opaque_faces_.clear();
  translucent_faces_.clear();

  num_cluster_not_visible_ = 0;
  num_not_in_frustum_ = 0;
  num_skipped_faces_ = 0;
  num_skipped_shaders_ = 0;

  int leafindex = find_leaf(camera_position);
  int cluster = leafs_[leafindex].cluster;

  already_visible_.reset();

  std::map<std::string, q3_shader*>::iterator it;

  for (int i = num_leafs_-1; i >= 0; --i)
  {
    if (!is_cluster_visible(cluster, leafs_[i].cluster)) 
    {
      ++num_cluster_not_visible_;
      continue;
    }

    glm::vec3 min((float)leafs_[i].mins[0], (float)leafs_[i].mins[1], (float)leafs_[i].mins[2]);
    glm::vec3 max((float)leafs_[i].maxs[0], (float)leafs_[i].maxs[1], (float)leafs_[i].maxs[2]);

    if (!g_frustum.box_in_frustum(min, max)) 
    {
      ++num_not_in_frustum_;
      continue;
    } 

    for (int j = leafs_[i].leafface+leafs_[i].num_leaffaces-1; j >= leafs_[i].leafface; --j) 
    {
      int face = leaffaces_[j].face;
      if (already_visible_.test(face))
      {
        ++num_skipped_faces_;
        continue;
      }
      already_visible_.set(face);

      //if (g_textures[faces[face].texture] == NULL) continue;
      //it = shaders.find(textures[faces[face].texture].name);
      //if (it->second->translucent)
      //{
      // 	m_translucent_faces.push_back(&(faces[face]));
      //} 
      // else 
      {
        opaque_faces_.push_back(&(faces_[face]));
      }
    }
  } 
}

bool faceSort(const bsp_face* left, const bsp_face* right)
{
  if (left->texture == right->texture) 
  {
    if (left->lm_index < right->lm_index) return true;
    return false;
  } 
  else if (left->texture < right->texture) return true;
  return false;
}

#define EPSILON 0.125
#define STEPSIZE 18
#define JUMP_VELOCITY 270

float   pm_stopspeed = 100.0f;
float   pm_duckScale = 0.25f;

float   pm_accelerate = 10.0f;
float   pm_airaccelerate = 1.0f;

float   pm_friction = 6.0f;
float   pm_spectatorfriction = 5.0f;

bool output_starts_out;
bool output_all_solid;
float output_fraction;
glm::vec4 output_plane;

float trace_radius = 20.0f;

float bsp::trace(glm::vec4& start, glm::vec4& end, glm::vec4* plane)
{
  output_starts_out = true;
  output_all_solid = false;
  output_fraction = 1.0f;

  check_node(0, 0.0f, 1.0f, start, end);

  *plane = output_plane;

  return output_fraction;

  if (output_fraction == 1.0f)
  {
    end = end;
  }
  else
  {
    end = start + output_fraction * (end - start);
  }
}

void bsp::check_node(int index, float start_fraction, float end_fraction, glm::vec4 start, glm::vec4 end)
{
  if (index < 0)
  {
    const bsp_leaf& leaf = leafs_[-(index + 1)];

    for (int i = 0; i < leaf.num_leafbrushes; ++i)
    {
      const bsp_brush& brush = brushes_[leafbrushes_[leaf.leafbrush + i].brush];

      if (brush.num_brushsides > 0 && textures_[brush.texture].contents & 1) // 1 == CONTENTS_SOLID
      {
        check_brush(brush, start, end);
      }
    }

    return;
  }

  const bsp_node& node = nodes_[index];
  const bsp_plane& plane = planes_[node.plane];

  glm::vec4 normal(plane.normal, 0.0f);

  const float start_distance = glm::dot(normal, start) - plane.distance;
  const float end_distance = glm::dot(normal, end) - plane.distance;

  float offset = trace_radius;

  if (start_distance >= offset && end_distance >= offset) // both in front of plane 
  {
    check_node(node.front, start_fraction, end_fraction, start, end);
  } 
  else if (start_distance < -offset && end_distance < -offset) // both behind the plane
  {
    check_node(node.back, start_fraction, end_fraction, start, end);
  }
  else 
  {
    int side;
    float fraction1, fraction2, middle_fraction;
    glm::vec4 middle;

    if (start_distance < end_distance)
    {
      side = 1;
      float inverse_distance = 1.0f / (start_distance - end_distance);
      fraction1 = (start_distance - offset + EPSILON) * inverse_distance;
      fraction2 = (start_distance + offset + EPSILON) * inverse_distance;
    }
    else if (end_distance < start_distance)
    {
      side = 0;
      float inverse_distance = 1.0f / (start_distance - end_distance);
      fraction1 = (start_distance + offset + EPSILON) * inverse_distance;
      fraction2 = (start_distance - offset - EPSILON) * inverse_distance;
    }
    else
    {
      side = 0;
      fraction1 = 1.0f;
      fraction2 = 0.0f;
    }

    if (fraction1 < 0.0f) fraction1 = 0.0f;
    if (fraction1 > 1.0f) fraction1 = 1.0f;

    middle_fraction = start_fraction + (end_fraction - start_fraction) * fraction1;
    middle = start + fraction1 * (end - start);

    if (side == 0)
      check_node(node.front, start_fraction, middle_fraction, start, middle);
    else
      check_node(node.back, start_fraction, middle_fraction, start, middle);

    if (fraction2 < 0.0f) fraction2 = 0.0f;
    if (fraction2 > 1.0f) fraction2 = 1.0f;

    middle_fraction = start_fraction + (end_fraction - start_fraction) * fraction2;
    middle = start + fraction2 * (end - start);

    if (!side == 0)
      check_node(node.front, middle_fraction, end_fraction, middle, end);
    else
      check_node(node.back, middle_fraction, end_fraction, middle, end);
  }
}       

void bsp::check_brush(const bsp_brush& brush, glm::vec4 start, glm::vec4 end)
{
  float start_fraction = -1.0f;
  float end_fraction = 1.0f;
  bool starts_out = false;
  bool ends_out = false;

  glm::vec4 clip_plane;

  for (int i = 0; i < brush.num_brushsides; ++i)
  {
    bsp_brushside& brushside = brushsides_[brush.brushside + i];
    bsp_plane& plane = planes_[brushside.plane]; 

    glm::vec4 normal(plane.normal, 0.0);

    //const float start_distance = glm::dot(normal, start) - plane.distance;
    //const float end_distance = glm::dot(normal, end) - plane.distance; 

    const float start_distance = glm::dot(normal, start) - (plane.distance + trace_radius);
    const float end_distance = glm::dot(normal, end) - (plane.distance + trace_radius); 

    if (start_distance > 0)
    {
      starts_out = true;
    }
    if (end_distance > 0)
    {
      ends_out = true;
    }

    if (start_distance > 0 && end_distance > 0) // both in front of plane 
    {
      return;
    } 
    else if (start_distance <= 0 && end_distance <= 0) // both behind the plane
    {
      continue;
    }

    if (start_distance > end_distance) // we enter a brush (start distance positive is in front of plane)
    {
      float fraction = (start_distance - EPSILON) / (start_distance - end_distance);

      if (fraction < 0.0f) fraction = 0.0f;

      if (fraction > start_fraction)
      {
        start_fraction = fraction;
        clip_plane = glm::vec4(plane.normal, plane.distance);
      }
    }
    else
    {
      float fraction = (start_distance + EPSILON) / (start_distance - end_distance);

      if (fraction > 1.0f) fraction = 1.0f;

      if (fraction < end_fraction)
      {
        end_fraction = fraction;
      }
    }  
  }

  if (starts_out == false)
  {
    output_starts_out = false;
    if (ends_out == false)
    {
      output_all_solid = true;
      output_fraction = 0.0f;
    }
    return;
  }

  if (start_fraction < end_fraction)
  {
    if (start_fraction > -1.0f && start_fraction < output_fraction)
    {
      if (start_fraction < 0.0f)
      {
        start_fraction = 0.0f;
      }
      output_fraction = start_fraction;
      output_plane = clip_plane;
    }
  }
}

