#ifndef BSP_H_
#define BSP_H_

#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class bezier;
class Q3Shader;

#define CONTENTS_SOLID 1 // an eye is never valid in a solid
#define CONTENTS_LAVA 8
#define CONTENTS_SLIME 16
#define CONTENTS_WATER 32
#define CONTENTS_FOG 64

#define CONTENTS_AREAPORTAL 0x8000

#define CONTENTS_PLAYERCLIP 0x10000
#define CONTENTS_MONSTERCLIP 0x20000
// bot specific contents types
#define CONTENTS_TELEPORTER 0x40000
#define CONTENTS_JUMPPAD 0x80000
#define CONTENTS_CLUSTERPORTAL 0x100000
#define CONTENTS_DONOTENTER 0x200000

#define CONTENTS_ORIGIN 0x1000000 // removed before bsping an entity

#define CONTENTS_BODY 0x2000000 // should never be on a brush, only in game
#define CONTENTS_CORPSE 0x4000000
#define CONTENTS_DETAIL 0x8000000      // brushes not used for the bsp
#define CONTENTS_STRUCTURAL 0x10000000 // brushes used for the bsp
#define CONTENTS_TRANSLUCENT                                                   \
  0x20000000 // don't consume surface fragments inside
#define CONTENTS_TRIGGER 0x40000000
#define CONTENTS_NODROP                                                        \
  0x80000000 // don't leave bodies or items (death fog, lava)

#define SURF_NODAMAGE 0x1 // never give falling damage
#define SURF_SLICK 0x2    // effects game physics
#define SURF_SKY 0x4      // lighting from environment map
#define SURF_LADDER 0x8
#define SURF_NOIMPACT 0x10     // don't make missile explosions
#define SURF_NOMARKS 0x20      // don't leave missile marks
#define SURF_FLESH 0x40        // make flesh sounds and effects
#define SURF_NODRAW 0x80       // don't generate a drawsurface at all
#define SURF_HINT 0x100        // make a primary bsp splitter
#define SURF_SKIP 0x200        // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHTMAP 0x400  // surface doesn't need a lightmap
#define SURF_POINTLIGHT 0x800  // generate lighting info at vertexes
#define SURF_METALSTEPS 0x1000 // clanking footsteps
#define SURF_NOSTEPS 0x2000    // no footstep sounds
#define SURF_NONSOLID 0x4000   // don't collide against curves with this set
#define SURF_LIGHTFILTER 0x8000  // act as a light filter during q3map -light
#define SURF_ALPHASHADOW 0x10000 // do per-pixel light shadow casting in q3map
#define SURF_NODLIGHT 0x20000    // never add dynamic lights

// content masks
#define MASK_ALL (-1)
#define MASK_SOLID (CONTENTS_SOLID)
#define MASK_PLAYERSOLID (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY)
#define MASK_DEADSOLID (CONTENTS_SOLID | CONTENTS_PLAYERCLIP)
#define MASK_WATER (CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME)
#define MASK_OPAQUE (CONTENTS_SOLID | CONTENTS_SLIME | CONTENTS_LAVA)
#define MASK_SHOT (CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE)

// plane types are used to speed some tests
// 0-2 are axial planes
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_NON_AXIAL 3

#define PlaneTypeForNormal(x)                                                  \
  (x[0] == 1.0                                                                 \
       ? PLANE_X                                                               \
       : (x[1] == 1.0 ? PLANE_Y : (x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL)))

enum lump {
  // Lump Name           Description
  LUMP_ENTITIES,    // Game-related object descriptions.
  LUMP_TEXTURES,    // Surface descriptions.
  LUMP_PLANES,      // Planes used by map geometry.
  LUMP_NODES,       // BSP tree nodes.
  LUMP_LEAFS,       // BSP tree leaves.
  LUMP_LEAFFACES,   // Lists of face indices, one list per leaf.
  LUMP_LEAFBRUSHES, // Lists of brush indices, one list per leaf.
  LUMP_MODELS,      // Descriptions of rigid world geometry in map.
  LUMP_BRUSHES,     // Convex polyhedra used to describe solid space.
  LUMP_BRUSHSIDES,  // Brush surfaces.
  LUMP_VERTEXES,    // Vertices used to describe faces.
  LUMP_MESHVERTS,   // Lists of offsets, one list per mesh.
  LUMP_EFFECTS,     // List of special map effects.
  LUMP_FACES,       // Surface geometry.
  LUMP_LIGHTMAPS,   // Packed lightmap data.
  LUMP_LIGHTVOLS,   // Local illumination data.
  LUMP_VISDATA      // Cluster-cluster visibility data.
};

enum faceType { POLYGON = 1, PATCH, MESH, BILLBOARD };

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
  char *ents;
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
  bsp_vertex operator+(const bsp_vertex &v) const {
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
  bsp_vertex operator*(float factor) const {
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
  unsigned char *vecs; // [num_vecs*size_vecs]
};

struct trace_info {
  bool starts_out;
  bool all_solid;
  float fraction;
  glm::vec4 plane;
};

class Bsp {
public:
  Bsp(void);
  ~Bsp(void);

  Bsp(std::string filename);

  int FindLeaf(const glm::vec4 &camera_position);
  std::vector<bsp_face *> ComputeVisibleFaces(const glm::vec4 &camera_position);
  bool IsClusterVisible(int cluster, int test_cluster);

  void load_lightmaps();

  float trace(glm::vec4 &start, glm::vec4 &end, trace_info &trace);
  void check_node(int index, float start_fraction, float end_fraction,
                  glm::vec4 start, glm::vec4 end);
  void check_brush(const bsp_brush &brush, glm::vec4 start, glm::vec4 end);

  GLuint vboId;
  GLuint iboId;

  std::map<bsp_face *, std::vector<bezier *>> patches_;

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
  bsp_entities *entities_;
  bsp_texture *textures_;
  bsp_plane *planes_;
  bsp_node *nodes_;
  bsp_leaf *leafs_;
  bsp_leafface *leaffaces_;
  bsp_leafbrush *leafbrushes_;
  bsp_model *models_;
  bsp_brush *brushes_;
  bsp_brushside *brushsides_;
  bsp_vertex *vertexes_;
  bsp_meshvert *meshverts_;
  bsp_effect *effects_;
  bsp_face *faces_;
  bsp_lightmap *lightmaps_;
  bsp_lightvol *lightvols_;
  bsp_visdata *visdata_;
};

#endif /* _BSP_H_ */
