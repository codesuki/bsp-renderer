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
      stage->blendfunc[0] = GL_ONE;
      stage->blendfunc[1] = GL_ZERO;
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

        //std::cout << "map: " << shader->stages[i]->map << std::endl;
      }
    }
  }
  //std::cout << "finished checking shaders" << std::endl;         

  load_lightmaps();

  //std::cout << "finished loading lightmaps" << std::endl;
#ifdef __USE_SHADERS__
  {
  std::map<std::string, q3_shader*>::iterator it;
  
  for (int i = 0; i < m_num_textures; ++i) 
  {
    std::map<std::string, q3_shader*>::iterator it;
    it = m_shaders.find(m_textures[i].name);
   
    std::cout << "processing: " << it->first << std::endl;
    it->second->compile();
  }
  }
#endif

#ifdef __USE_VBO__
  // calculate correct buffer size
  // all vertexes + all tesselated beziers
  int num_bezier_vertexes = 0;
  int num_bezier_indexes = 0;
   
  std::map<bsp_face*, std::vector<bezier*> >::iterator it;  

  for (it = m_patches.begin(); it != m_patches.end(); ++it)
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
      m_header.direntries[LUMP_VERTEXES].length + num_bezier_vertexes*sizeof(bsp_vertex), 
      NULL, 
      GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, m_header.direntries[LUMP_VERTEXES].length, m_vertexes);

  glGenBuffers(1, &iboId);

  // bind VBO in order to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // upload data to VBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
      m_header.direntries[LUMP_MESHVERTS].length + num_bezier_indexes*sizeof(unsigned int), 
      NULL, 
      GL_STATIC_DRAW); 

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_header.direntries[LUMP_MESHVERTS].length, m_meshverts); 
  
  int offset_verts = 0;
  int offset_idx = 0;
  for (it = m_patches.begin(); it != m_patches.end(); ++it)
  {
    for (int j = 0; j < it->second.size(); ++j)
    {
      glBufferSubData(GL_ARRAY_BUFFER,
          m_header.direntries[LUMP_VERTEXES].length + offset_verts, 
          11*11*sizeof(bsp_vertex), 
          it->second[j]->m_vertexes);
      
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
          m_header.direntries[LUMP_MESHVERTS].length + offset_idx,
          10*11*2*sizeof(unsigned int), 
          it->second[j]->m_indexes); 

      it->second[j]->m_vertex_offset = m_header.direntries[LUMP_VERTEXES].length + offset_verts;
      it->second[j]->m_index_offset = m_header.direntries[LUMP_MESHVERTS].length + offset_idx;
      
      offset_verts += sizeof(bsp_vertex)*11*11;
      offset_idx += sizeof(unsigned int)*10*11*2;    
    }
  } 
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
                   if (token.compare("add") == 0 || 
                       token.compare("ADD") == 0 || 
                       token.compare("Add") == 0 || 
                       token.compare("GL_add") == 0) 
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

void bsp::get_visible_faces(const vec3f& camera_position)
{
  m_opaque_faces.clear();
  m_translucent_faces.clear();

  m_num_cluster_not_visible = 0;
  m_num_not_in_frustum = 0;
  m_num_skipped_faces = 0;
  m_num_skipped_shaders = 0;

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
      //it = m_shaders.find(m_textures[m_faces[face].texture].name);
      //if (it->second->translucent)
      //{
      // 	m_translucent_faces.push_back(&(m_faces[face]));
      //} 
     // else 
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

void bsp::render(const vec3f& camera_position)
{
  get_visible_faces(camera_position);
  //std::cout << "visible faces: " << m_opaque_faces.size() << std::endl;
  std::sort(m_opaque_faces.begin(), m_opaque_faces.end(), faceSort);

  std::map<std::string, q3_shader*>::iterator it;

  for (int i = 0; i < m_opaque_faces.size(); ++i) 
  {
    render_face(m_opaque_faces[i]);
  }
}


void bsp::end_shader(const q3_shader& shader)
{
#ifndef __USE_SHADERS__
  for (int i = 0; i < shader.stages.size(); ++i)
  {
    glActiveTexture(GL_TEXTURE0+i);
    glDisable(GL_TEXTURE_2D);
  }
#endif  
  glDisable(GL_BLEND);
}

void my_tex_env_mode(unsigned int texture_unit, unsigned int new_mode) 
{
  static unsigned int mode[8] = {0};

  if (mode[texture_unit] != new_mode)
  {
    mode[texture_unit] = new_mode;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, new_mode);
  }
}

void my_bind_texture(unsigned int texture_unit, unsigned int new_texture)
{
  static unsigned int texture[8] = {0};

  if (texture[texture_unit] != new_texture)
  {
    texture[texture_unit] = new_texture;
#ifdef __USE_SHADERS___ // not supported yet
    glBindTexture(new_texture);
#else
    glBindTexture(GL_TEXTURE_2D, new_texture);
#endif
  }
}

void my_blend_func(unsigned int new_blend_func0, unsigned int new_blend_func1)
{
  static unsigned int blend_func[2] = {0};

  if (blend_func[0] != new_blend_func0 || blend_func[1] != new_blend_func1)
  {
    blend_func[0] = new_blend_func0;
    blend_func[1] = new_blend_func1;
    glBlendFunc(new_blend_func0, new_blend_func1);
  }
}

void my_alpha_func(unsigned int new_alpha_func, unsigned int new_alpha_val)
{
  static unsigned int alpha_func = 0;

  if (alpha_func != new_alpha_func)
  {
    alpha_func = new_alpha_func;
    glAlphaFunc(new_alpha_func, new_alpha_val);
  }
}

void my_active_texture(unsigned int new_active_texture)
{ 
  static int active_texture = -1;

  if (active_texture != new_active_texture)
  {
    active_texture = new_active_texture;
    glActiveTexture(new_active_texture);
  } 
}

void bsp::prepare_shader(q3_shader& shader, int offset, int lm_index)
{ 
  static q3_shader* current_shader = 0;

  const bsp_vertex& vertex = m_vertexes[offset];
  
  if (&shader == current_shader)
  {
    for (int i = 0; i < shader.stages.size(); ++i)
    {
      const q3_shader_stage& stage = *(shader.stages[i]);
      my_active_texture(GL_TEXTURE0+i);
#ifndef __USE_SHADERS__
      glClientActiveTexture(GL_TEXTURE0+i);
#endif 
      if (stage.map.compare("$lightmap") == 0)
      {
        my_bind_texture(i, lightmaps_[lm_index]);
#ifndef __USE_SHADERS__
#ifdef __USE_VBO__
        glTexCoordPointer(2, 
            GL_FLOAT, 
            sizeof(bsp_vertex), 
            BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)+sizeof(vec2f)));
#else
        glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.lmcoord));
#endif
#endif
      }
      else
      {
#ifndef __USE_SHADERS__
#ifdef __USE_VBO__
        glTexCoordPointer(2,
            GL_FLOAT, 
            sizeof(bsp_vertex),
            BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)));
#else
        glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.texcoord));
#endif
#endif
      } 
    }
    ++m_num_skipped_shaders;
    return;
  } 

  if (current_shader != 0)
    end_shader(*current_shader);

  current_shader = &shader;

  for (int i = 0; i < shader.stages.size(); ++i)
  {
    const q3_shader_stage& stage = *(shader.stages[i]);

    // only enable blending in stage 0 to blend with background
    // shaders can only blend with textures
    if (i == 0 && stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    {
      glEnable(GL_BLEND);
      my_blend_func(stage.blendfunc[0], stage.blendfunc[1]);
    }
   
    my_active_texture(GL_TEXTURE0+i);
#ifndef __USE_SHADERS__
    glClientActiveTexture(GL_TEXTURE0+i);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
     
    if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ZERO)
    {  
      my_tex_env_mode(i, GL_REPLACE);
    }
    else if (stage.blendfunc[0] == GL_DST_COLOR && stage.blendfunc[1] == GL_ZERO)
    {
      my_tex_env_mode(i, GL_MODULATE);
    }
    else if (stage.blendfunc[0] == GL_SRC_ALPHA &&
        stage.blendfunc[1] == GL_ONE_MINUS_SRC_ALPHA)
    {
      my_tex_env_mode(i, GL_DECAL);
    }
    else if (stage.blendfunc[0] == GL_ONE && stage.blendfunc[1] == GL_ONE)
    {
      my_tex_env_mode(i, GL_ADD);
    }
    else
    {
      my_tex_env_mode(i, GL_REPLACE);
    }
#endif

    if (stage.map.compare("$lightmap") == 0)
    {
      my_bind_texture(i, lightmaps_[lm_index]);
#ifndef __USE_SHADERS__
#ifdef __USE_VBO__
      glTexCoordPointer(2, 
          GL_FLOAT, 
          sizeof(bsp_vertex), 
          BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)+sizeof(vec2f)));
#else
      glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.lmcoord));
#endif
#endif
    }
    else
    {
      my_bind_texture(i, stage.texture);
#ifndef __USE_SHADERS__
#ifdef __USE_VBO__
      glTexCoordPointer(2,
          GL_FLOAT, 
          sizeof(bsp_vertex),
          BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)));
#else
      glTexCoordPointer(2, GL_FLOAT, sizeof(bsp_vertex), &(vertex.texcoord));
#endif
#endif
    } 
  } 
}

void bsp::render_face(bsp_face* face)
{
  const bsp_face &current_face = *face;
  static const int stride = sizeof(bsp_vertex); 
  const int offset = current_face.vertex;
 
  std::map<std::string, q3_shader*>::iterator it;
  it = m_shaders.find(m_textures[current_face.texture].name);
  q3_shader& shader = *(it->second);

  prepare_shader(shader, offset, current_face.lm_index);

#ifdef __USE_SHADERS__
  static unsigned int current_shader = 0;
  if (current_shader != shader.shader)
  {
    glUseProgram(shader.shader);
    current_shader = shader.shader;
  }      
#endif
   
  if (face->type == POLYGON || face->type == MESH) 
  {  
    if (offset >= m_num_vertexes) return;
#ifdef __USE_VBO__
#ifdef __USE_SHADERS__     
    glEnableVertexAttribArray(shader.position_idx);   
    glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex)));
    
    glEnableVertexAttribArray(shader.tex_coord_idx);   
    glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)));
    
    glEnableVertexAttribArray(shader.lm_coord_idx);   
    glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex)+sizeof(vec3f)+sizeof(vec2f)));

   /* glEnableVertexAttribArray(shader.color_idx);
    glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, sizeof(bsp_vertex), 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex) + sizeof(float)*10));  */ 
#else
    glVertexPointer(3, GL_FLOAT, stride, 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex)));

    glColorPointer(4, GL_BYTE, stride, 
        BUFFER_OFFSET(offset*sizeof(bsp_vertex)+2*sizeof(vec3f)+2*sizeof(vec2f)));
#endif
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
  } 
  else if (face->type == PATCH) 
  { 
   /* 
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

      prepare_shader(*(it->second), 0, current_face.lm_index);

      glUseProgram(it->second->shader);
      
      glMultiDrawElements(GL_TRIANGLE_STRIP, (GLsizei*)b->m_tri_per_row, GL_UNSIGNED_INT, (const GLvoid**)&(b->m_row_indexes), 10);
      
      end_shader(*(it->second));
      */
    std::vector<bezier*> patches = m_patches[face];

    for (int i = 0; i < patches.size(); ++i) 
    {
      const bezier* b = patches[i];

      glEnableVertexAttribArray(shader.position_idx);   
      glVertexAttribPointer(shader.position_idx, 3, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(b->m_vertex_offset));
    
      glEnableVertexAttribArray(shader.tex_coord_idx);   
      glVertexAttribPointer(shader.tex_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(b->m_vertex_offset+sizeof(vec3f)));
    
      glEnableVertexAttribArray(shader.lm_coord_idx);   
      glVertexAttribPointer(shader.lm_coord_idx, 2, GL_FLOAT, GL_FALSE, stride, 
        BUFFER_OFFSET(b->m_vertex_offset+sizeof(vec3f)+sizeof(vec2f)));

      /*glEnableVertexAttribArray(shader.color_idx);
      glVertexAttribPointer(shader.color_idx, 4, GL_BYTE, GL_FALSE, stride, 
        BUFFER_OFFSET(b->m_vertex_offset+sizeof(float)*10));  */
     
      prepare_shader(shader, 0, current_face.lm_index);

      for (int j = 0; j < 10; j++) 
      {
        glDrawElements(GL_TRIANGLE_STRIP, 
            11*2,
            GL_UNSIGNED_INT, 
            BUFFER_OFFSET(b->m_index_offset+sizeof(unsigned int)*j*11*2));
      }
    } 
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
  if (stages.size() == 0)
    return;

  std::cout << "mh" << std::endl;

  std::stringstream vertex_shader;
  vertex_shader << "attribute vec4 inPosition;\n" 
    << "attribute vec2 inTexCoord;\n"
    << "attribute vec2 inLmCoord;\n"
    << "attribute vec3 inColor;\n"
    << "varying vec2 outTexCoord;\n"
    << "varying vec2 outLmCoord;\n";

  vertex_shader << "void main() {\n"
    << "\toutTexCoord = inTexCoord;\n"
    << "\toutLmCoord = inLmCoord;\n"
    //<< "\tgl_FrontColor = inColor;\n"
    << "\tgl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * inPosition;\n";
  
  std::stringstream fragment_shader;
  
  for (int i = 0; i < stages.size(); ++i)
  {
    //vertex_shader << "\tgl_TexCoord[" << i << "] = gl_MultiTexCoord" << i << ";\n";
    fragment_shader << "uniform sampler2D texture" << i << "; //" << stages[i]->map << "\n";
  }

  fragment_shader << "varying vec2 outTexCoord;\n"
    << "varying vec2 outLmCoord;\n";

  fragment_shader << "void main() {\n";

  for (int i = 0; i < stages.size(); ++i)
  {
    if (!stages[i]->map.compare("$lightmap"))
    {  
      fragment_shader << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", outLmCoord.st);\n";
    }   
    else
    {   
      fragment_shader << "\tvec4 color" << i 
        << " = texture2D(texture" << i << ", outTexCoord.st);\n";
    }
  }

  std::string dst;
  std::string src;

  for (int i = 0; i < stages.size(); ++i)
  {
    const q3_shader_stage& stage = *(stages[i]);

    std::stringstream helper;
    helper << "color" << i;
    src = helper.str();

    if (i == 0) 
    {
      dst = "gl_FragColor";
    }
    else
    {
      std::stringstream helper;
      helper << "color" << i-1;
      dst = helper.str();
    }  
    
    // 0.50 equals 128 when normalized to 0-1 range
    if (stage.alphafunc != 0)
    {
      if (stage.alphafunc == GL_GREATER)
      {
        fragment_shader << "if (" << src << ".a > 0) discard;\n";
      }
      else if (stage.alphafunc == GL_LESS)
      {
        fragment_shader << "if (" << src << ".a > 0.501960785) discard;\n";
      }
      else
      {
        fragment_shader << "if (" << src << ".a <= 0.501960785) discard;\n";
      }
    }

    fragment_shader << "\t" << src << " = (" << src << " * ";

    switch (stage.blendfunc[0])
    {
      case GL_ONE:
        fragment_shader << "1";
        break;
      case GL_ZERO:
        fragment_shader << "0";
        break;
      case GL_DST_COLOR:
        fragment_shader << dst;
        break;
      case GL_SRC_COLOR:
        fragment_shader << src;
        break;
      case GL_DST_ALPHA:
        fragment_shader << dst << ".a";
        break;
      case GL_SRC_ALPHA:
        fragment_shader << src << ".a";
        break;
      case GL_ONE_MINUS_SRC_ALPHA:
        fragment_shader << "(1 - " << src << ".a)";
        break;
      case GL_ONE_MINUS_DST_ALPHA:
        fragment_shader << "(1 - " << dst << ".a)";
        break;
      default:
        std::cout << stage.map << " :: " << stage.blendfunc[0] << std::endl; 
    }

    fragment_shader << ") + (" << dst << " * "; 

    switch (stage.blendfunc[1])
    {
      case GL_ONE:
        fragment_shader << "1";
        break;
      case GL_ZERO:
        fragment_shader << "0";
        break;
      case GL_DST_COLOR:
        fragment_shader << dst;
        break;
      case GL_SRC_COLOR:
        fragment_shader << src;
        break;
      case GL_DST_ALPHA:
        fragment_shader << dst << ".a";
        break;
      case GL_SRC_ALPHA:
        fragment_shader << src << ".a";
        break;
      case GL_ONE_MINUS_SRC_ALPHA:
        fragment_shader << "(1 - " << src << ".a)";
        break;
      case GL_ONE_MINUS_DST_ALPHA:
        fragment_shader << "(1 - " << dst << ".a)";
        break;                  
      default:
        std::cout << stage.map << " :: " << stage.blendfunc[1] << std::endl; 
    }

    fragment_shader << ");\n";    
  }   

  fragment_shader << "\t" << "gl_FragColor = " << src << ";\n";  

  vertex_shader << "}";
  fragment_shader << "}";

  std::replace(name.begin(), name.end(), '/', '-');

  std::ofstream ofs;

  std::cout << "ASDSAD " << name << std::endl;

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

  texture_idx[0] = glGetUniformLocation(shader, "texture0"); 
  texture_idx[1] = glGetUniformLocation(shader, "texture1"); 
  texture_idx[2] = glGetUniformLocation(shader, "texture2"); 
  texture_idx[3] = glGetUniformLocation(shader, "texture3"); 
  texture_idx[4] = glGetUniformLocation(shader, "texture4"); 
  texture_idx[5] = glGetUniformLocation(shader, "texture5"); 
  texture_idx[6] = glGetUniformLocation(shader, "texture6"); 
  texture_idx[7] = glGetUniformLocation(shader, "texture7"); 

  position_idx = glGetAttribLocation(shader, "inPosition");
  tex_coord_idx = glGetAttribLocation(shader, "inTexCoord");
  lm_coord_idx = glGetAttribLocation(shader, "inLmCoord");   
  color_idx = glGetAttribLocation(shader, "inColor");

  glUseProgram(shader);
  for (int i = 0; i < 8; ++i)
  {
    glUniform1i(texture_idx[i], i);
  }  
  glUseProgram(0);
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

    fprintf(stdout, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
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
    fprintf(stdout, "Linker failure: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  return program;
}


