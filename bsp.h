#ifndef __BSP_H__
#define __BSP_H__

#include "util.h"

class bezier;
class Q3Shader;

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

class Bsp
{
public:
  Bsp(void);
  ~Bsp(void);

  Bsp(std::string filename);

  int find_leaf(const glm::vec4& camera_position);
  void get_visible_faces(const glm::vec4& camera_position);

  bool is_cluster_visible(int cluster, int test_cluster);

  void load_shaders();


  void load_lightmaps();

  float trace(glm::vec4& start, glm::vec4& end, glm::vec4* plane);
  void check_node(int index, float start_fraction, float end_fraction, glm::vec4 start, glm::vec4 end);
  void check_brush(const bsp_brush& brush, glm::vec4 start, glm::vec4 end);

  GLuint* lightmaps_;
  GLuint vboId;
  GLuint iboId;

  std::map<std::string, Q3Shader*> shaders_;
  float time_;

  std::bitset<10000> already_visible_;
  std::map<bsp_face*, std::vector<bezier*> > patches_;
  std::vector<bsp_face*> opaque_faces_;
  std::vector<bsp_face*> translucent_faces_;

  // some status variables for outputting info
  int num_cluster_not_visible_;
  int num_not_in_frustum_;
  int num_skipped_faces_;
  int num_skipped_shaders_;

  int num_entities_;
  int num_textures_;
  int num_planes_;
  int num_nodes_;
  int num_leafs_;
  int num_leaffaces_;
  int num_leafbrushes_;
  int num_models_;
  int num_brushes_;
  int num_brushsides_;
  int num_vertexes_;
  int num_meshverts_;
  int num_effects_;
  int num_faces_;
  int num_lightmaps_;
  int num_lightvols_;

  bsp_header header_;
  bsp_entities* entities_;
  bsp_texture* textures_;
  bsp_plane* planes_;
  bsp_node* nodes_;
  bsp_leaf* leafs_;
  bsp_leafface* leaffaces_;
  bsp_leafbrush* leafbrushes_;
  bsp_model* models_;
  bsp_brush* brushes_;
  bsp_brushside* brushsides_;
  bsp_vertex* vertexes_;
  bsp_meshvert* meshverts_;
  bsp_effect* effects_;
  bsp_face* faces_;
  bsp_lightmap* lightmaps_;
  bsp_lightvol* lightvols_;
  bsp_visdata* visdata_;
};

#endif /* _BSP_H_ */ 
