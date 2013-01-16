#ifndef _BSP_H_
#define _BSP_H_

#include "util.h"

class bezier;

#define PlaneTypeForNormal(x) (x[0] == 1.0 ? PLANE_X : (x[1] == 1.0 ? PLANE_Y : (x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL) ) )

enum lump {
  //Lump Name           Description  
  LUMP_ENTITIES,      // Game-related object descriptions.  
  LUMP_TEXTURES,      // Surface descriptions.  
  LUMP_PLANES,        // Planes used by map geometry.  
  LUMP_NODES,           // BSP tree nodes.  
  LUMP_LEAFS,           // BSP tree leaves.  
  LUMP_LEAFFACES,       // Lists of face indices, one list per leaf.  
  LUMP_LEAFBRUSHES,     // Lists of brush indices, one list per leaf.  
  LUMP_MODELS,          // Descriptions of rigid world geometry in map.  
  LUMP_BRUSHES,         // Convex polyhedra used to describe solid space.  
  LUMP_BRUSHSIDES,      // Brush surfaces.  
  LUMP_VERTEXES,        // Vertices used to describe faces.  
  LUMP_MESHVERTS,       // Lists of offsets, one list per mesh.  
  LUMP_EFFECTS,         // List of special map effects.  
  LUMP_FACES,           // Surface geometry.  
  LUMP_LIGHTMAPS,       // Packed lightmap data.  
  LUMP_LIGHTVOLS,       // Local illumination data.  
  LUMP_VISDATA          // Cluster-cluster visibility data.
};

enum faceType {
  POLYGON = 1,
  PATCH,
  MESH,
  BILLBOARD
};

struct bsp_direntry {
  int offset;
  int length;
};

struct bsp_header {
  char magic[4];
  int version;
  bsp_direntry direntries[17]; 
};

struct bsp_entities {
  char* ents;
};

struct bsp_texture {
  char name[64];
  int flags;
  int contents;
};

struct bsp_plane {
  glm::vec3 normal;
  float distance;
};

struct bsp_node {
  int plane;
  int front;
  int back; 
  int mins[3];
  int maxs[3];
};

struct bsp_leaf {
  int cluster;
  int area;
  int mins[3];
  int maxs[3];
  int leafface; // first leafface
  int num_leaffaces; 
  int leafbrush; // first leafbrush
  int num_leafbrushes;
};

struct bsp_leafface {
  int face;
};

struct bsp_leafbrush {
  int brush;
};

struct bsp_model {
  float mins[3];
  float maxs[3];
  int face; // first face
  int num_faces;
  int brush; // first brush
  int num_brushes;
};

struct bsp_brush {
  int brushside; // first brushside
  int num_brushsides;
  int texture;
};

struct bsp_brushside {
  int plane;
  int texture;
};

// maybe wrong? -> q3 source
class bsp_vertex {
public:
  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec2 lmcoord;
  glm::vec3 normal;
  unsigned char color[4];

  /**
  Used for bezier patch tesselation
  */
  bsp_vertex operator+(const bsp_vertex& v) const {
    bsp_vertex res;

    res.position = position + v.position;
    res.texcoord = texcoord + v.texcoord;
    res.lmcoord = lmcoord + v.lmcoord;
    res.normal = normal + v.normal;

    return res;
  }

  /**
  Used for bezier patch tesselation
  */
  bsp_vertex operator*(float factor) const      {
    bsp_vertex res;

    res.position = position * factor;
    res.texcoord = texcoord * factor;
    res.lmcoord = lmcoord * factor;
    res.normal = normal * factor;

    return res;
  }
};



struct bsp_meshvert {
  unsigned int offset;
};

struct bsp_effect {
  char name[64];
  int brush;
  int unknown;
};

// hier vllt fehler bei lm_vecs (q3 source..)
struct bsp_face {
  int texture;
  int effect;
  int type;
  int vertex; // first vertex
  int num_vertices;
  int meshvert; // first (index?) meshvert
  int num_meshverts;
  int lm_index;
  int lm_start[2];
  int lm_size[2];
  glm::vec3 lm_origin;
  glm::vec3 lm_vecs[2];
  glm::vec3 normal;
  int size[2];
};

struct bsp_lightmap {
  unsigned char map[128][128][3];
};

struct bsp_lightvol {
  unsigned char ambient[3];
  unsigned char directional[3];
  unsigned char dir[2];
};

struct bsp_visdata {
  int num_vecs;
  int size_vecs;
  unsigned char* vecs; // [num_vecs*size_vecs]
};

enum wavefunc 
{
  WAVEFUNC_NONE,
  WAVEFUNC_SIN,
  WAVEFUNC_TRIANGLE,
  WAVEFUNC_SQUARE,
  WAVEFUNC_SAWTOOTH,
  WAVEFUNC_INVERSESAWTOOTH
};

enum alphafunc
{
  ALPHAFUNC_NONE,
  ALPHAFUNC_GREATER,
  ALPHAFUNC_LESS,
  ALPHAFUNC_GEQUAL
};

enum rgbgen_t
{
  RGBGEN_WAVE,
  RGBGEN_IDENTITY,
  RGBGEN_VERTEX,
  RGBGEN_EXACTVERTEX
};

#define MAX_TEXMODS 4

enum tcmod
{
  TCMOD_NONE,
  TCMOD_SCROLL,
  TCMOD_SCALE,
  TCMOD_TURB,
  TCMOD_TRANSFORM,
  TCMOD_STRETCH,
  TCMOD_ROTATE
};

struct waveform
{ 
  wavefunc type;
  float base;
  float amplitude;
  float phase;
  float frequency; 
};

struct texmod
{
  tcmod type;

  waveform wave;

  float scale[2];
  float scroll[2];

  float matrix[2][2];
  float translate[2];

  float rotate_speed;

  texmod()
  {
    type = TCMOD_NONE;
  };
};

struct q3_shader_stage {
  GLuint texture;
  bool translucent;
  std::string map;
  int blendfunc[2];
  int alphafunc;
  bool depthwrite;
  bool clamp;

  rgbgen_t rgbgen;
  waveform rgbwave;

  int num_texmods;
  texmod texmods[MAX_TEXMODS];

  q3_shader_stage() 
  {
    texture = NULL;
    translucent = false;
    blendfunc[0] = -1;
    blendfunc[1] = -1;
    alphafunc = NULL;
    depthwrite = false;
    rgbgen = RGBGEN_IDENTITY;
    num_texmods = 0;
    clamp = false;
  };
};

class q3_shader {
public:
  q3_shader(void);
  ~q3_shader(void);

  void compile();

  std::vector<q3_shader_stage*> stages;

  bool translucent;
  GLuint shader;
  std::string name;

  int texture_idx[8];
  int position_idx;
  int tex_coord_idx;
  int lm_coord_idx;
  int color_idx;
  int time_idx;
  int projection_idx;
  int model_idx;
};

class bsp
{
public:
  bsp(void);
  ~bsp(void);

  bsp(std::string filename);

  int find_leaf(const glm::vec4& camera_position);
  void get_visible_faces(const glm::vec4& camera_position);

  void render(const glm::vec4& camera_position, float time);
  bool is_cluster_visible(int cluster, int test_cluster);
  void render_face(bsp_face* face);

  void load_shaders();
  int parse_shader_stage(const std::string* shader, int offset, q3_shader_stage* stage);
  std::map<std::string, q3_shader*> m_shaders;

  void load_lightmaps();
  void prepare_shader(q3_shader& shader, int offset, int lmindex);
  void end_shader(const q3_shader& shader);


  float trace(glm::vec4& start, glm::vec4& end);
  void check_node(int index, float start_fraction, float end_fraction, glm::vec4 start, glm::vec4 end);
  void check_brush(const bsp_brush& brush, glm::vec4 start, glm::vec4 end);

  std::bitset<10000> m_already_visible;
  std::map<bsp_face*, std::vector<bezier*> > m_patches;
  std::vector<bsp_face*> m_opaque_faces;
  std::vector<bsp_face*> m_translucent_faces;

  GLuint* lightmaps_;
  GLuint vboId;
  GLuint iboId;

  float m_time;

  // some status variables for outputting info
  int m_num_cluster_not_visible;
  int m_num_not_in_frustum;
  int m_num_skipped_faces;
  int m_num_skipped_shaders;

  int m_num_entities;
  int m_num_textures;
  int m_num_planes;
  int m_num_nodes;
  int m_num_leafs;
  int m_num_leaffaces;
  int m_num_leafbrushes;
  int m_num_models;
  int m_num_brushes;
  int m_num_brushsides;
  int m_num_vertexes;
  int m_num_meshverts;
  int m_num_effects;
  int m_num_faces;
  int m_num_lightmaps;
  int m_num_lightvols;

  bsp_header m_header;
  bsp_entities* m_entities;
  bsp_texture* m_textures;
  bsp_plane* m_planes;
  bsp_node* m_nodes;
  bsp_leaf* m_leafs;
  bsp_leafface* m_leaffaces;
  bsp_leafbrush* m_leafbrushes;
  bsp_model* m_models;
  bsp_brush* m_brushes;
  bsp_brushside* m_brushsides;
  bsp_vertex* m_vertexes;
  bsp_meshvert* m_meshverts;
  bsp_effect* m_effects;
  bsp_face* m_faces;
  bsp_lightmap* m_lightmaps;
  bsp_lightvol* m_lightvols;
  bsp_visdata* m_visdata;
};

#endif /* _BSP_H_ */ 
