#include "bsp.h"
#include "util.h"
#include "texture.h"
#include "bezier.h"

bsp::bsp(void)
{

}

bsp::~bsp(void)
{
  SAFE_DELETE_ARRAY(m_entities->ents);
  SAFE_DELETE(m_entities);
  SAFE_DELETE_ARRAY(m_textures);
  SAFE_DELETE_ARRAY(m_planes);
  SAFE_DELETE_ARRAY(m_nodes);
  SAFE_DELETE_ARRAY(m_planes);
  SAFE_DELETE_ARRAY(m_leafs);
  SAFE_DELETE_ARRAY(m_leaffaces);
  SAFE_DELETE_ARRAY(m_leafbrushes);
  SAFE_DELETE_ARRAY(m_models);
  SAFE_DELETE_ARRAY(m_brushes);
  SAFE_DELETE_ARRAY(m_brushsides);
  SAFE_DELETE_ARRAY(m_vertexes);
  SAFE_DELETE_ARRAY(m_meshverts);
  SAFE_DELETE_ARRAY(m_effects);
  SAFE_DELETE_ARRAY(m_faces);
  SAFE_DELETE_ARRAY(m_lightmaps);
  SAFE_DELETE_ARRAY(m_lightvols);
  SAFE_DELETE_ARRAY(m_visdata->vecs);
  SAFE_DELETE(m_visdata);

  {
    std::map<bsp_face*, std::vector<bezier*> >::iterator it;
    for (it = m_patches.begin(); it != m_patches.end(); ++it)
      for (int i = 0; i < (*it).second.size(); ++i)
        SAFE_DELETE(((*it).second)[i]);
  }

  {
    std::map<std::string, q3_shader*>::iterator it;
    for (it = m_shaders.begin(); it != m_shaders.end(); ++it) 
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

  fin.read((char*)&m_header, sizeof(bsp_header));

  m_entities = new bsp_entities;
  m_entities->ents = new char[m_header.direntries[LUMP_ENTITIES].length];

  m_num_textures = m_header.direntries[LUMP_TEXTURES].length/sizeof(bsp_texture);
  m_num_planes = m_header.direntries[LUMP_PLANES].length/sizeof(bsp_plane);
  m_num_nodes = m_header.direntries[LUMP_NODES].length/sizeof(bsp_node);
  m_num_planes = m_header.direntries[LUMP_PLANES].length/sizeof(bsp_plane);
  m_num_leafs = m_header.direntries[LUMP_LEAFS].length/sizeof(bsp_leaf);
  m_num_leaffaces = m_header.direntries[LUMP_LEAFFACES].length/sizeof(bsp_leafface);
  m_num_leafbrushes = m_header.direntries[LUMP_LEAFBRUSHES].length/sizeof(bsp_leafbrush);
  m_num_models = m_header.direntries[LUMP_MODELS].length/sizeof(bsp_model);
  m_num_brushes = m_header.direntries[LUMP_BRUSHES].length/sizeof(bsp_brush);
  m_num_brushsides = m_header.direntries[LUMP_BRUSHSIDES].length/sizeof(bsp_brushside);
  m_num_vertexes = m_header.direntries[LUMP_VERTEXES].length/sizeof(bsp_vertex);
  m_num_meshverts = m_header.direntries[LUMP_MESHVERTS].length/sizeof(bsp_meshvert);
  m_num_effects = m_header.direntries[LUMP_EFFECTS].length/sizeof(bsp_effect);
  m_num_faces = m_header.direntries[LUMP_FACES].length/sizeof(bsp_face);
  m_num_lightmaps = m_header.direntries[LUMP_LIGHTMAPS].length/sizeof(bsp_lightmap);
  m_num_lightvols = m_header.direntries[LUMP_LIGHTVOLS].length/sizeof(bsp_lightvol);

  m_textures = new bsp_texture[m_num_textures];
  m_nodes = new bsp_node[m_num_nodes];
  m_planes = new bsp_plane[m_num_planes];
  m_leafs = new bsp_leaf[m_num_leafs];
  m_leaffaces = new bsp_leafface[m_num_leaffaces];
  m_leafbrushes = new bsp_leafbrush[m_num_leafbrushes];
  m_models = new bsp_model[m_num_models];
  m_brushes = new bsp_brush[m_num_brushes];
  m_brushsides = new bsp_brushside[m_num_brushsides];
  m_vertexes = new bsp_vertex[m_num_vertexes];
  m_meshverts = new bsp_meshvert[m_num_meshverts];
  m_effects = new bsp_effect[m_num_effects];
  m_faces = new bsp_face[m_num_faces];
  m_lightmaps = new bsp_lightmap[m_num_lightmaps];
  m_lightvols = new bsp_lightvol[m_num_lightvols];
  m_visdata = new bsp_visdata;

  fin.seekg(m_header.direntries[LUMP_ENTITIES].offset);
  fin.read((char*)m_entities->ents, m_header.direntries[LUMP_ENTITIES].length);

  fin.seekg(m_header.direntries[LUMP_TEXTURES].offset);
  fin.read((char*)m_textures, m_header.direntries[LUMP_TEXTURES].length);

  fin.seekg(m_header.direntries[LUMP_PLANES].offset);
  fin.read((char*)m_planes, m_header.direntries[LUMP_PLANES].length);

  fin.seekg(m_header.direntries[LUMP_NODES].offset);
  fin.read((char*)m_nodes, m_header.direntries[LUMP_NODES].length);

  fin.seekg(m_header.direntries[LUMP_LEAFS].offset);
  fin.read((char*)m_leafs, m_header.direntries[LUMP_LEAFS].length);

  fin.seekg(m_header.direntries[LUMP_LEAFFACES].offset);
  fin.read((char*)m_leaffaces, m_header.direntries[LUMP_LEAFFACES].length);

  fin.seekg(m_header.direntries[LUMP_LEAFBRUSHES].offset);
  fin.read((char*)m_leafbrushes, m_header.direntries[LUMP_LEAFBRUSHES].length);

  fin.seekg(m_header.direntries[LUMP_MODELS].offset);
  fin.read((char*)m_models, m_header.direntries[LUMP_MODELS].length);

  fin.seekg(m_header.direntries[LUMP_BRUSHES].offset);
  fin.read((char*)m_brushes, m_header.direntries[LUMP_BRUSHES].length);

  fin.seekg(m_header.direntries[LUMP_BRUSHSIDES].offset);
  fin.read((char*)m_brushsides, m_header.direntries[LUMP_BRUSHSIDES].length);

  fin.seekg(m_header.direntries[LUMP_VERTEXES].offset);
  fin.read((char*)m_vertexes, m_header.direntries[LUMP_VERTEXES].length);

  fin.seekg(m_header.direntries[LUMP_MESHVERTS].offset);
  fin.read((char*)m_meshverts, m_header.direntries[LUMP_MESHVERTS].length);

  fin.seekg(m_header.direntries[LUMP_EFFECTS].offset);
  fin.read((char*)m_effects, m_header.direntries[LUMP_EFFECTS].length);

  fin.seekg(m_header.direntries[LUMP_FACES].offset);
  fin.read((char*)m_faces, m_header.direntries[LUMP_FACES].length);

  fin.seekg(m_header.direntries[LUMP_LIGHTMAPS].offset);
  fin.read((char*)m_lightmaps, m_header.direntries[LUMP_LIGHTMAPS].length);

  fin.seekg(m_header.direntries[LUMP_LIGHTVOLS].offset);
  fin.read((char*)m_lightvols, m_header.direntries[LUMP_LIGHTVOLS].length);

  fin.seekg(m_header.direntries[LUMP_VISDATA].offset);
  fin.read((char*)m_visdata, 2*sizeof(int));
  m_visdata->vecs = new unsigned char[m_visdata->num_vecs*m_visdata->size_vecs];
  fin.read((char*)m_visdata->vecs, sizeof(unsigned char)*m_visdata->num_vecs*m_visdata->size_vecs);

  fin.close();

  // prepare patch data + tesselate
  for (int i = 0; i < m_num_faces; ++i) 
  {
    if (m_faces[i].type == PATCH) 
    {
      bsp_face* face = &(m_faces[i]);

      int width         = m_faces[i].size[0];
      int height        = m_faces[i].size[1];
      int widthCount    = (width - 1) / 2;
      int heightCount   = (height - 1) / 2;

      m_patches[face].resize(widthCount*heightCount);
      for (int j = 0; j < widthCount*heightCount; ++j) 
      {
        m_patches[face][j] = new bezier();
      }

      for (int y = 0; y < heightCount; y++) 
      {
        for (int x = 0; x < widthCount; x++) 
        {
          for (int row = 0; row < 3; row++) 
          {
            for (int col = 0; col < 3; col++) 
            {
              m_patches[face][y * widthCount + x]->m_control_vertexes[row * 3 + col] =
                m_vertexes[m_faces[i].vertex +(y * 2 * width + x * 2)+row * width + col];
            }
          }
          m_patches[face][y * widthCount + x]->tessellate(10);
        }
      }
    }
  }
  /*
     for (int i = 0; i < m_num_vertexes; ++i) 
     {
     m_vertexes[i].color[0] = 0xff;
     m_vertexes[i].color[1] = 0xff;
     m_vertexes[i].color[2] = 0xff;
     m_vertexes[i].color[3] = 0xff;
     }
     */
  load_shaders();

  //std::cout << "checking shaders" << std::endl;

  for (int i = 0; i < m_num_textures; ++i) 
  {
    std::map<std::string, q3_shader*>::iterator it;
    it = m_shaders.find(m_textures[i].name);

    //std::cout << "checking: " << m_textures[i].name << std::endl;

    if (it == m_shaders.end()) 
    {
      //std::cout << "not found -> creating " << std::endl;

      q3_shader* shader = new q3_shader();
      shader->translucent = false;
      q3_shader_stage* stage = new q3_shader_stage;
      int res = Texture::LoadTexture(m_textures[i].name, (stage)); 
      stage->map = m_textures[i].name;
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

      m_shaders.insert(std::pair<std::string, q3_shader*>(m_textures[i].name, shader));

      //std::cout << "-> created" << std::endl;
    } 
    else 
    {
      q3_shader *shader = it->second;
      shader->translucent = false;
      //std::cout << "shader found [" << shader->stages.size() << "]-> loading textures" << std::endl;

      for (int i = 0; i < shader->stages.size(); ++i) 
      {
        //std::cout << "stage " << i << std::endl; 
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

        //std::cout << "map: " << shader->stages[i]->map << std::endl;
      }
    }
  }
  //std::cout << "finished checking shaders" << std::endl;         

  load_lightmaps();

  //std::cout << "finished loading lightmaps" << std::endl;
  std::map<std::string, q3_shader*>::iterator it;

  for (it = m_shaders.begin(); it != m_shaders.end(); ++it)
  {
    std::cout << "processing: " << it->first << std::endl;
    it->second->compile();
  }

#ifdef __USE_VBO__
  // generate a new VBO and get the associated ID
  glGenBuffers(1, &vboId);

  // bind VBO in order to use
  glBindBuffer(GL_ARRAY_BUFFER, vboId);

  // upload data to VBO
  glBufferData(GL_ARRAY_BUFFER, 
      m_header.direntries[LUMP_VERTEXES].length, 
      m_vertexes, 
      GL_STATIC_DRAW);

  glGenBuffers(1, &iboId);

  // bind VBO in order to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // upload data to VBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
      m_header.direntries[LUMP_MESHVERTS].length, 
      m_meshverts, 
      GL_STATIC_DRAW); 
#endif
}

void bsp::load_lightmaps()
{
  lightmaps_ = new GLuint[m_num_lightmaps];
  for (int i = 0; i < m_num_lightmaps; ++i)
  {
    Texture::loadLightmap(m_lightmaps[i], &(lightmaps_[i]));  
  }
}

// shouldn't be global
std::string g_shaders = "";

int read_shader(const char *file_name, const struct stat *file_info, int file_type)
{
  if (file_type != FTW_F) return 0;

  const char *filetype = file_name+strlen(file_name)-7;

  if (strcmp(filetype, ".shader")) return 0;

  char* buffer = new char[file_info->st_size];

  std::string filename = "";
  filename.append(file_name);
  std::ifstream ifs(filename.c_str(), std::ios::in);

  ifs.read(buffer, file_info->st_size);
  ifs.close();

  g_shaders.append(buffer, ifs.gcount());
  SAFE_DELETE_ARRAY(buffer);

  return 0;
}

void bsp::load_shaders()
{
  // load all shaders into g_shaders
  ftw("scripts/", &read_shader, 1);

  //std::cout << "shader buffer length: " << g_shaders.length() << std::endl;

  bool is_shader = false;
  std::string name = "";
  q3_shader* current_shader;

  for (int i = 0; i < g_shaders.length(); ++i) 
  {
    switch (g_shaders[i]) 
    {
      case '/':
        if (g_shaders[i+1] == '/') 
        {
          i = g_shaders.find("\r\n", i);
          break;
        } 
        else 
        {
          name.append(1, '/');
          break;
        }
      case '{':
        if (is_shader == true) 
        {
          // sub-shader found
          q3_shader_stage* stage = new q3_shader_stage;
          i = parse_shader_stage(&g_shaders, i, stage);
          current_shader->stages.push_back(stage);
        } 
        else 
        {
          current_shader = new q3_shader();
          current_shader->translucent = false;
          current_shader->name = name;
          m_shaders.insert(std::pair<std::string, q3_shader*>(name, current_shader));
          is_shader = true;

          //std::cout << "read shader: " << name << std::endl;
        }

        break;
      case '}':
        name = "";
        is_shader = false;
        break;
      case ' ':
        break;
      case 0x09: break;
      case 0x0A: break;
      case 0x0D: break;
      default:
                 name.append(1, g_shaders[i]);
    }
  }
}

int get_token_end_pos(const std::string* buffer, int offset)
{
  while (1) 
  {
    switch ((*buffer)[offset]) 
    {
      case ' ': 
        return offset;
      case 0x09: 
        return offset;
      case 0x0A: 
        return offset;
      case 0x0D: 
        return offset;
    }
    ++offset;
  }
}

int get_blend_func(std::string name) {
  if (strcasecmp("GL_SRC_ALPHA", name.c_str()) == 0) 
  {
    return GL_SRC_ALPHA;
  } 
  else if (strcasecmp("GL_ONE_MINUS_SRC_ALPHA", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_SRC_ALPHA;
  } 
  else if (strcasecmp("GL_ONE", name.c_str()) == 0) 
  {
    return GL_ONE;
  } 
  else if (strcasecmp("GL_ZERO", name.c_str()) == 0) 
  {
    return GL_ZERO;
  } 
  else if (strcasecmp("GL_DST_COLOR", name.c_str()) == 0) 
  {
    return GL_DST_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_DST_COLOR", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_DST_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_SRC_COLOR", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_SRC_COLOR;
  } 
  else if (strcasecmp("GL_ONE_MINUS_DST_ALPHA", name.c_str()) == 0) 
  {
    return GL_ONE_MINUS_DST_ALPHA;
  } 
  else if (strcasecmp("GL_SRC_COLOR", name.c_str()) == 0) 
  {
    return GL_SRC_COLOR;
  }
  else 
  {
    std::cout << "ERROR: " << name << std::endl;
  }
}

std::string blend_func_to_string(int blendfunc)
{
  switch (blendfunc)
  {
    case GL_SRC_ALPHA:
      return std::string("GL_SRC_ALPHA");
      break;
    case GL_ONE_MINUS_SRC_ALPHA:
      return std::string("GL_ONE_MINUS_SRC_ALPHA");
      break;
    case GL_ONE:
      return std::string("GL_ONE");
      break;
    case GL_ZERO:
      return std::string("GL_ZERO");
      break;
    case GL_DST_COLOR:
      return std::string("GL_DST_COLOR");
      break;
    case GL_ONE_MINUS_DST_COLOR:
      return std::string("GL_ONE_MINUS_DST_COLOR");
      break;
    case GL_ONE_MINUS_SRC_COLOR:
      return std::string("GL_ONE_MINUS_SRC_COLOR");
      break;
    case GL_ONE_MINUS_DST_ALPHA:
      return std::string("GL_ONE_MINUS_DST_ALPHA");
      break;
    case GL_SRC_COLOR:
      return std::string("GL_SRC_COLOR");
      break;
    default:
      return std::string("anders");
  }
}

int get_alpha_func(std::string name) 
{
  if (strcasecmp("GT0", name.c_str()) == 0) 
  {
    return GL_GREATER;
  } 
  else if (strcasecmp("LT128", name.c_str()) == 0) 
  {
    return GL_LESS;
  } 
  else if (strcasecmp("GE128", name.c_str()) == 0) 
  {
    return GL_GEQUAL;
  }
}

int bsp::parse_shader_stage(const std::string* shaders, int offset, q3_shader_stage* stage)
{
  int i = offset;
  for (; i < shaders->length(); ++i) 
  {
    switch ((*shaders)[i]) 
    {
      case '{': break;
      case '}': return i;
      case ' ': break;
      case 0x09: break;
      case 0x0A: break;
      case 0x0D: break;
      default:
                 int end_pos = get_token_end_pos(shaders, i);
                 std::string token = shaders->substr(i, end_pos-i);
                 i = ++end_pos;

                 if (strcasecmp("map", token.c_str()) == 0 || strcasecmp("clampmap", token.c_str()) == 0) 
                 {
                   end_pos = shaders->find("\r\n", i);
                   token = shaders->substr(i, end_pos-i);
                   i = end_pos;
                   stage->map = token;
                   stage->map.erase(0, stage->map.find_first_not_of(' '));
                   i = shaders->find("\r\n", i);
                 } 
                 else if (strcasecmp("blendfunc", token.c_str()) == 0) 
                 {
                   end_pos = get_token_end_pos(shaders, i);
                   token = shaders->substr(i, end_pos-i);
                   i = ++end_pos;
                   if (token.compare("add") == 0) 
                   {
                     stage->blendfunc[0] = get_blend_func("GL_ONE");
                     stage->blendfunc[1] = get_blend_func("GL_ONE");
                   } 
                   else if (token.compare("filter") == 0)
                   {
                     stage->blendfunc[0] = get_blend_func("GL_DST_COLOR");
                     stage->blendfunc[1] = get_blend_func("GL_ONE");
                   } 
                   else if (token.compare("blend") == 0) 
                   {
                     stage->blendfunc[0] = get_blend_func("GL_SRC_ALPHA");
                     stage->blendfunc[1] = get_blend_func("GL_ONE_MINUS_SRC_ALPHA");
                   } 
                   else 
                   {
                     stage->blendfunc[0] = get_blend_func(token);
                     end_pos = get_token_end_pos(shaders, i);
                     token = shaders->substr(i, end_pos-i);
                     i = end_pos;
                     stage->blendfunc[1] = get_blend_func(token);
                   }
                 } 
                 else if (strcasecmp("alphafunc", token.c_str()) == 0) 
                 {
                   end_pos = get_token_end_pos(shaders, i);
                   token = shaders->substr(i, end_pos-i);
                   i = ++end_pos;
                   stage->alphafunc = get_alpha_func(token);
                 } 
                 else if (strcasecmp("tcmod", token.c_str()) == 0) 
                 {
                   end_pos = get_token_end_pos(shaders, i);
                   token = shaders->substr(i, end_pos-i);
                   i = ++end_pos;
                   if (strcasecmp("scroll", token.c_str()) == 0) 
                   {
                     end_pos = get_token_end_pos(shaders, i);
                     token = shaders->substr(i, end_pos-i);
                     i = ++end_pos;
                     stage->scroll[0] = atof(token.c_str());
                     end_pos = get_token_end_pos(shaders, i);
                     token = shaders->substr(i, end_pos-i);
                     i = ++end_pos;
                     stage->scroll[1] = atof(token.c_str());
                   } 
                   else if (strcasecmp("scale", token.c_str()) == 0) 
                   {
                     end_pos = get_token_end_pos(shaders, i);
                     token = shaders->substr(i, end_pos-i);
                     i = ++end_pos;
                     stage->scale[0] = atof(token.c_str());
                     end_pos = get_token_end_pos(shaders, i);
                     token = shaders->substr(i, end_pos-i);
                     i = ++end_pos;
                     stage->scale[1] = atof(token.c_str());
                   }
                 } 
                 else 
                 {
                   i = shaders->find("\r\n", i);
                 }
    }
  }
  return i;
}

int bsp::find_leaf(const vec3f& camera_position)
{
  int index = 0;

  while (index >= 0) 
  {
    const bsp_node& node = m_nodes[index];
    const bsp_plane& plane = m_planes[node.plane];

    // TODO: multiplicate the plane with our transformation matrices (inverse and transpose needed for plane transformation)
    // TODO: maybe do this during load time to enhance framerate! 

    // Distance from point to a plane

    mat4f transform_matrix;
    vec3f transformed_vector = plane.normal;

    mat4f inverse_matrix;

    transform_matrix = quake2oglMatrix;
    /*compute_inverse(transform_matrix, inverse_matrix);
      transform_matrix = transpose(inverse_matrix);

      transformed_vector = transform_matrix * transformed_vector;

      bsp_plane transformed_plane;
      transformed_plane.normal = transformed_vector;

      bsp_plane test_plane = plane;
      float temp = test_plane.normal[1];
      test_plane.normal[1] = test_plane.normal[2];
      test_plane.normal[2] = -temp;
      */
    vec3f pos = transform_matrix * camera_position;

    const float distance = plane.normal.dot(pos) - plane.distance;

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
  if ((m_visdata->vecs == NULL) || (cluster < 0)) return true;

  int i = (cluster * m_visdata->size_vecs) + (test_cluster >> 3);
  unsigned char visSet = m_visdata->vecs[i];

  if (!(visSet & (1 << (test_cluster & 7)))) return false;
  return true;
}

frustum_cullerf culler;
//culler.setup(projectionMatrix);

void bsp::get_visible_faces(const vec3f& camera_position)
{
  m_opaque_faces.clear();
  m_translucent_faces.clear();

  m_num_cluster_not_visible = 0;
  m_num_not_in_frustum = 0;
  m_num_skipped_faces = 0;

  int leafindex = find_leaf(camera_position);
  int cluster = m_leafs[leafindex].cluster;

  m_already_visible.reset();

  std::map<std::string, q3_shader*>::iterator it;

  for (int i = m_num_leafs-1; i >= 0; --i)
  {
    if (!is_cluster_visible(cluster, m_leafs[i].cluster)) 
    {
      ++m_num_cluster_not_visible;
      continue;
    }

    // if (!g_frustum.box_in_frustum(&D3DXVECTOR3(m_leafs[i].mins[0], m_leafs[i].mins[1], m_leafs[i].mins[2]), &D3DXVECTOR3(m_leafs[i].maxs[0], m_leafs[i].maxs[1], m_leafs[i].maxs[2]))) {
    //   ++m_num_not_in_frustum;
    //   continue;
    // }

    for (int j = m_leafs[i].leafface+m_leafs[i].num_leaffaces-1; j >= m_leafs[i].leafface; --j) 
    {
      int face = m_leaffaces[j].face;
      if (m_already_visible.test(face))
      {
        ++m_num_skipped_faces;
        continue;
      }
      m_already_visible.set(face);

      //if (g_textures[m_faces[face].texture] == NULL) continue;
      it = m_shaders.find(m_textures[m_faces[face].texture].name);
      //  if (it->second->translucent)
      {
        //  	m_translucent_faces.push_back(&(m_faces[face]));
      } 
      //  else 
      {
        m_opaque_faces.push_back(&(m_faces[face]));
      }
    }
  } 

  //std::cout << "current leaf index: " << leafindex << std::endl;
  //std::cout << "current cluster: " << cluster << std::endl;
  //std::cout << "not in cluster: " << m_num_cluster_not_visible << std::endl;
  //std::cout << "skipped faces:  " << m_num_skipped_faces << std::endl;
  //std::cout << "total faces:    " << m_num_faces << std::endl;
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

bool g_blend = false;

void bsp::render(const vec3f& camera_position)
{
  get_visible_faces(camera_position);
  //std::cout << "visible faces: " << m_opaque_faces.size() << std::endl;
  std::sort(m_opaque_faces.begin(), m_opaque_faces.end(), faceSort);

  int current_texture = -1;
  int current_lightmap = -2;

  std::map<std::string, q3_shader*>::iterator it;

  for (int i = 0; i < m_opaque_faces.size(); ++i) 
  {
    // if (current_texture != m_opaque_faces[i]->texture) 
    // {
    //   current_texture = m_opaque_faces[i]->texture;
    //   it = m_shaders.find(m_textures[(m_opaque_faces[i]->texture)].name);	
    // } 
    // else 
    // { 
    //   continue; 
    // }

    render_face(m_opaque_faces[i]);
  }

  //pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  //pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  //pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  //pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  //for (int i = 0; i < m_translucent_faces.size(); ++i) {
  //	render_face(m_translucent_faces[i]);
  //}
}


void bsp::end_shader(const q3_shader& shader)
{
  for (int i = 0; i < shader.stages.size(); ++i)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB+i);
    glClientActiveTextureARB(GL_TEXTURE0_ARB+i);
    glBindTexture(GL_TEXTURE_2D, 0); 
    glDisable(GL_TEXTURE_2D);  
    glDisable(GL_ALPHA_TEST);
    //glEnable(GL_CULL_FACE);
  }
}

void bsp::prepare_shader(const q3_shader& shader, int offset, int lm_index)
{  
  const bsp_vertex& vertex = m_vertexes[offset];

  for (int i = 0; i < shader.stages.size(); ++i)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB+i);
    glClientActiveTextureARB(GL_TEXTURE0_ARB+i);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);

    const q3_shader_stage& stage = *(shader.stages[i]);

    if (stage.alphafunc != 0)
    {
      glEnable(GL_ALPHA_TEST);

      //glDisable(GL_CULL_FACE);
      if (stage.alphafunc == GL_GREATER)
      {
        glAlphaFunc(GL_GREATER, 0);
      }
      else
      {
        glAlphaFunc(stage.alphafunc, 128);
      }
    }

    if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ZERO)
    {  
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
    else if (stage.blendfunc[0] == GL_DST_COLOR && stage.blendfunc[1] == GL_ZERO)
    {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    else if (stage.blendfunc[0] == GL_SRC_ALPHA &&
        stage.blendfunc[1] == GL_ONE_MINUS_SRC_ALPHA)
    {
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_DECAL);
    }
    else if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    {
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
    }
    else
    {
      //std::cout << stage.map << ":" << (stage.blendfunc[0]) << " and " << (stage.blendfunc[1]) << std::endl;
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }

    if (stage.map.compare("$lightmap") == 0)
    {
      glBindTexture(GL_TEXTURE_2D, lightmaps_[lm_index]);
#ifdef __USE_VBO__
      glTexCoordPointer(2, 
          GL_FLOAT, 
          sizeof(bsp_vertex), 
          BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)+sizeof(vec2f)));
#else
      glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.lmcoord));
#endif
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, stage.texture);
#ifdef __USE_VBO__
      glTexCoordPointer(2,
          GL_FLOAT, 
          sizeof(bsp_vertex),
          BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)));
#else
      glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.texcoord));
#endif
    } 
  } 
}

void bsp::render_face(bsp_face* face)
{
  //glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  //glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);  

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  const bsp_face &current_face = *face;
  static const int stride = sizeof(bsp_vertex); // BSP Vertex, not float[3]
  const int offset = current_face.vertex;

  //if (face->type == MESH)
  //{
  //         std::map<std::string, q3_shader*>::iterator it;
  //  it = m_shaders.find(m_textures[current_face.texture].name);
  //
  //  std::cout << m_textures[current_face.texture].name << std::endl;
  //}

  if (face->type == POLYGON /*|| face->type == MESH*/) 
  {
    if (offset >= m_num_vertexes) return;

    std::map<std::string, q3_shader*>::iterator it;
    it = m_shaders.find(m_textures[current_face.texture].name);

    prepare_shader(*(it->second), offset, current_face.lm_index);

    glUseProgram(it->second->shader);

#ifdef __USE_VBO__
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexPointer(3, GL_FLOAT, stride, BUFFER_OFFSET(offset*sizeof(bsp_vertex)));
    glColorPointer(4, GL_BYTE, stride, BUFFER_OFFSET(offset*sizeof(bsp_vertex)+2*sizeof(vec3f) + 2*sizeof(vec2f)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glDrawElements(GL_TRIANGLES, 
        current_face.num_meshverts, 
        GL_UNSIGNED_INT, 
        BUFFER_OFFSET(current_face.meshvert * sizeof(bsp_meshvert))); 
#else 
    glVertexPointer(3, GL_FLOAT, stride, &(m_vertexes[offset].position));
    glColorPointer(4, GL_BYTE, stride, &(m_vertexes[offset].color));
    glDrawElements(GL_TRIANGLES, 
        current_face.num_meshverts,
        GL_UNSIGNED_INT, 
        &(m_meshverts[current_face.meshvert])); 
#endif

    glUseProgram(0);

    end_shader(*(it->second));
  } 
  else if (face->type == PATCH) 
  { /* 
       std::vector<bezier*> patches = m_patches[face];

       std::map<std::string, q3_shader*>::iterator it;
       it = m_shaders.find(m_textures[current_face.texture].name);

       for (int i = 0; i < patches.size(); ++i) 
       {
       const bezier* b = patches[i];
       const bsp_vertex* vertexes = b->m_vertexes;
       const unsigned int* indexes = b->m_indexes;

       glColorPointer(4, GL_BYTE, stride, &(vertexes[0].color));
       glVertexPointer(3, GL_FLOAT, sizeof(bsp_vertex), vertexes[0].position);

       prepare_shader(*(it->second), vertexes[0], current_face.lm_index);

       for (int j = 0; j < 10; j++) 
       {
       glDrawElements(GL_TRIANGLE_STRIP, 11*2, GL_UNSIGNED_INT, &(indexes[11*j*2]));
       }

       end_shader(*(it->second));
       } */ 
  } //else if (m_faces[face].type == BILLBOARD) {
}

q3_shader::q3_shader()
{
}

q3_shader::~q3_shader() 
{

}

void q3_shader::compile()
{ 
  std::stringstream vertex_shader;
  vertex_shader << "void main() {\n gl_FrontColor = gl_Color;\n gl_Position = ftransform();\n";
  
  std::stringstream fragment_shader;
  
  for (int i = 0; i < stages.size(); ++i)
  {
    vertex_shader << "gl_TexCoord[" << i << "] = gl_MultiTexCoord" << i << ";\n";
    fragment_shader << "uniform sampler2D texture" << i << ";\n";
  }

  fragment_shader << "void main() {\n";
  
  for (int i = 0; i < stages.size(); ++i)
  {
    fragment_shader << "vec4 color" << i << " = texture2D(texture" << i << ", gl_TexCoord[" << i << "].st);\n";
  } 
  
  for (int i = 0; i < stages.size(); ++i)
  {
    const q3_shader_stage& stage = *(stages[i]);

    std::string last_texture;

    std::stringstream helper;
    helper << "color" << i;
    std::string current_texture = helper.str();

    if (i == 0) 
    {
      last_texture = "gl_FragColor";
    }
    else
    {
      std::stringstream helper;
      helper << "color" << i-1;
      last_texture = helper.str();
    }

    if (stage.alphafunc != 0)
    {
      if (stage.alphafunc == GL_GREATER)
      {
        glAlphaFunc(GL_GREATER, 0);
      }
      else
      {
        glAlphaFunc(stage.alphafunc, 128);
      }
    }

    if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ZERO)
    { 
      // replace 
      fragment_shader << current_texture << " = " << current_texture << ";\n";
    }
    else if (stage.blendfunc[0] == GL_DST_COLOR && stage.blendfunc[1] == GL_ZERO)
    {
      // modulate
      fragment_shader << current_texture << " = vec4(" << current_texture << ".rgb * " << last_texture << ".rgb, " << current_texture << ".a * " << last_texture << ".a);\n"; 
    }
    else if (stage.blendfunc[0] == GL_SRC_ALPHA &&
        stage.blendfunc[1] == GL_ONE_MINUS_SRC_ALPHA)
    {
      // decal
      fragment_shader << current_texture << " = vec4(" << last_texture << ".rgb * (1-" << current_texture << ".a) + " << current_texture << ".rgb * " << current_texture << ".a, " << last_texture << ".a);\n"; 
    }
    else if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    {
      // add
      fragment_shader << current_texture << " = "  << last_texture << " + " << current_texture << ";\n"; 
    }
    else if (stage.blendfunc[0] == GL_DST_COLOR && stage.blendfunc[1] == GL_SRC_ALPHA)
    {
      fragment_shader << current_texture << " = vec4(" << last_texture << ".rgb, " << current_texture << ".a);\n"; 
    }
    else
    {
      // replace 
      fragment_shader << current_texture << " = " << current_texture << ";\n"; 
    }

    fragment_shader << "gl_FragColor = " << current_texture << ";\n";
  } 

  vertex_shader << "}";
  fragment_shader << "}";

  std::replace( name.begin(), name.end(), '/', '-');

  std::ofstream ofs;

  std::string fname = "shaders/" + name + ".vert";
  ofs.open(fname.c_str());
  ofs << vertex_shader.str();
  ofs.close();

  fname = "shaders/" + name + ".frag";
  ofs.open(fname.c_str());
  ofs << fragment_shader.str();
  ofs.close();

  std::vector<GLuint> shaders;
  shaders.push_back(CreateShader(GL_VERTEX_SHADER, vertex_shader.str()));
  shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fragment_shader.str()));

  shader = CreateProgram(shaders);
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
  GLuint shader = glCreateShader(eShaderType);
  const char *strFileData = strShaderFile.c_str();
  glShaderSource(shader, 1, &strFileData, NULL);

  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

    const char *strShaderType = NULL;
    switch(eShaderType)
    {
      case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
      case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
      case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
    }

    fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
    delete[] strInfoLog;
  }

  return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
  GLuint program = glCreateProgram();

  for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
    glAttachShader(program, shaderList[iLoop]);

  glLinkProgram(program);

  GLint status;
  glGetProgramiv (program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
    fprintf(stderr, "Linker failure: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  return program;
}


