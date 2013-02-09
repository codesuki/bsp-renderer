#ifndef __MODEL_H__
#define __MODEL_H__

#include "util.h"

#define Q_PI 3.14159265358979323846

#define MD3_MAX_FRAMES 1024
#define MD3_MAX_TAGS 16
#define MD3_MAX_SURFACES 32
#define MD3_MAX_SHADERS	256
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
	float u,v;
};

class Model
{
public:
	Model(void);
	~Model(void);

	Model(std::string filename);

	md3_header m_header;

	md3_frame* m_frames;
	md3_tag* m_tags;
	md3_surface* m_surfaces;

	md3_shader** m_shaders;
	md3_triangle** m_triangles;
	md3_texcoord** m_texcoords;
	md3_vertex** m_normals;

	my_vertex** m_vertices;
};

#endif