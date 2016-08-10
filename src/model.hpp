#ifndef MODEL_H_
#define MODEL_H_

#include <string>

#include <glm/glm.hpp>

#include "logger.hpp"

class Shader;

#define Q_PI 3.14159265358979323846

#define MD3_MAX_FRAMES 1024
#define MD3_MAX_TAGS 16
#define MD3_MAX_SURFACES 32
#define MD3_MAX_SHADERS 256
#define MD3_MAX_VERTS 4096
#define MD3_MAX_TRIANGLES 8192

struct md3_header {
  int ident;
  int version;
  char name[64];
  int flags;
  int num_frames;
  int num_tags;
  int num_surfaces;
  int num_skins;
  int ofs_frames;
  int ofs_tags;
  int ofs_surfaces;
  int ofs_eof;
};

struct md3_frame {
  glm::vec3 min_bounds;
  glm::vec3 max_bounds;
  glm::vec3 local_origin;
  float radius;
  char name[16];
};

struct md3_tag {
  char name[64];
  glm::vec3 origin;
  glm::vec3 axis[3];
};

struct md3_surface {
  int ident;
  char name[64];
  int flags;
  int num_frames;
  int num_shaders;
  int num_verts;
  int num_triangles;
  int ofs_triangles;
  int ofs_shaders;
  int ofs_st;
  int ofs_xyznormal;
  int ofs_end;
};

struct md3_shader {
  char name[64];
  int shader_index;
};

struct md3_triangle {
  unsigned int indexes[3];
};

struct md3_texcoord {
  float st[2];
};

struct md3_vertex {
  short coord[3];
  short normal;
};

struct my_vertex {
  glm::vec3 position;
  glm::vec3 normal;
  float u, v;
};

enum animation {
  BOTH_DEATH1,
  BOTH_DEAD1,
  BOTH_DEATH2,
  BOTH_DEAD2,
  BOTH_DEATH3,
  BOTH_DEAD3,

  TORSO_GESTURE,
  TORSO_ATTACK,
  TORSO_ATTACK2,
  TORSO_DROP,
  TORSO_RAISE,
  TORSO_STAND,
  TORSO_STAND2,

  LEGS_WALKCR,
  LEGS_WALK,
  LEGS_RUN,
  LEGS_BACK,
  LEGS_SWIM,
  LEGS_JUMP,
  LEGS_LAND,
  LEGS_JUMPB,
  LEGS_LANDB,
  LEGS_IDLE,
  LEGS_IDLECR,
  LEGS_TURN,

  TORSO_GETFLAG,
  TORSO_GUARDBASE,
  TORSO_PATROL,
  TORSO_FOLLOWME,
  TORSO_AFFIRMATIVE,
  TORSO_NEGATIVE,

  MAX_ANIMATIONS,

  LEGS_BACKCR,
  LEGS_BACKWALK,
  FLAG_RUN,
  FLAG_STAND,
  FLAG_STAND2RUN,

  MAX_TOTALANIMATIONS
};

struct animation_info {
  unsigned int first_frame;
  unsigned int num_frames;
  unsigned int loop_frames;
  double frames_per_second;
  bool reversed;
};

class Model {
public:
  Model(void);
  ~Model(void);

  Model(std::string filename);

  glm::vec3 head_offset_;

  md3_header header_;

  md3_frame *frames_;
  md3_tag *tags_;
  md3_surface *surfaces_;

  md3_shader **shaders_;
  md3_triangle **triangles_;
  md3_texcoord **texcoords_;
  md3_vertex **normals_;

  my_vertex **vertices_;

  unsigned int vboId_;
  unsigned int iboId_;

  Shader *shader_;
};

class PlayerModel {
public:
  PlayerModel(std::string path, std::string name);
  void LoadAnimations();
  void ParseAnimations(char *buffer);

  animation_info animations_[MAX_TOTALANIMATIONS];

private:
  glm::vec3 head_offset_;

  // sex
  // footsteps

  Model head_;
  Model body_;
  Model legs_;

  std::string path_;
  std::string name_;
};

#endif
