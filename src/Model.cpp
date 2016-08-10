#include "model.hpp"

#include <fstream>

#include <boost/filesystem.hpp>

#include <GL/glew.h>

Model::Model(void) {
  // header_ = NULL;
  frames_ = nullptr;
  tags_ = nullptr;
  surfaces_ = nullptr;
  shaders_ = nullptr;
  triangles_ = nullptr;
  texcoords_ = nullptr;
  normals_ = nullptr;
  vertices_ = nullptr;
}

Model::~Model(void) {
  if (shaders_ != nullptr) {
    for (int i = 0; i < header_.num_surfaces; i++) {
      delete[] shaders_[i];
    }
    delete[] shaders_;
  }

  if (triangles_ != nullptr) {
    for (int i = 0; i < header_.num_surfaces; i++) {
      delete[] triangles_[i];
    }
    delete[] triangles_;
  }

  if (texcoords_ != nullptr) {
    for (int i = 0; i < header_.num_surfaces; i++) {
      delete[] texcoords_[i];
    }
    delete[] texcoords_;
  }

  if (normals_ != nullptr) {
    for (int i = 0; i < header_.num_surfaces; i++) {
      delete[] normals_[i];
    }
    delete[] normals_;
  }

  if (vertices_ != nullptr) {
    for (int i = 0; i < header_.num_surfaces; i++) {
      delete[] vertices_[i];
    }
    delete[] vertices_;
  }

  // if (header_ != NULL) delete header_;
  if (frames_ != nullptr)
    delete[] frames_;
  if (tags_ != nullptr)
    delete[] tags_;
  if (surfaces_ != nullptr)
    delete[] surfaces_;
}

// TODO: maybe free unused data like files' index and vertexes! (got my own
// structure for that)
Model::Model(std::string filename) {
  std::ifstream fin(filename.c_str(), std::ios::binary);

  fin.read((char *)(&header_), sizeof(md3_header));

  frames_ = new md3_frame[header_.num_frames];
  tags_ = new md3_tag[header_.num_tags * header_.num_frames];
  surfaces_ = new md3_surface[header_.num_surfaces];
  shaders_ = new md3_shader *[header_.num_surfaces];
  triangles_ = new md3_triangle *[header_.num_surfaces];
  texcoords_ = new md3_texcoord *[header_.num_surfaces];
  normals_ = new md3_vertex *[header_.num_surfaces];
  vertices_ = new my_vertex *[header_.num_surfaces];

  fin.seekg(header_.ofs_frames);
  for (int i = 0; i < header_.num_frames; ++i) {
    fin.read((char *)(&frames_[i]), sizeof(md3_frame));
  }

  fin.seekg(header_.ofs_tags);
  for (int i = 0; i < header_.num_tags * header_.num_frames; ++i) {
    fin.read((char *)(&tags_[i]), sizeof(md3_tag));
  }

  fin.seekg(header_.ofs_surfaces);
  for (int i = 0; i < header_.num_surfaces; ++i) {
    fin.read((char *)(&surfaces_[i]), sizeof(md3_surface));

    shaders_[i] = new md3_shader[surfaces_[i].num_shaders];
    triangles_[i] = new md3_triangle[surfaces_[i].num_triangles];
    texcoords_[i] =
        new md3_texcoord[(surfaces_[i].num_verts) * (surfaces_[i].num_frames)];
    normals_[i] =
        new md3_vertex[(surfaces_[i].num_verts) * (surfaces_[i].num_frames)];
    vertices_[i] =
        new my_vertex[(surfaces_[i].num_verts) * (surfaces_[i].num_frames)];

    fin.seekg(header_.ofs_surfaces + surfaces_[i].ofs_shaders);
    for (int j = 0; j < surfaces_[i].num_shaders; ++j) {
      fin.read((char *)(&shaders_[i][j]), sizeof(md3_shader));
    }

    fin.seekg(header_.ofs_surfaces + surfaces_[i].ofs_triangles);
    for (int j = 0, k = 0; j < surfaces_[i].num_triangles; ++j, k += 3) {
      fin.read((char *)(&triangles_[i][j]), sizeof(md3_triangle));
    }

    fin.seekg(header_.ofs_surfaces + surfaces_[i].ofs_st);
    for (int j = 0; j < surfaces_[i].num_verts; ++j) {
      fin.read((char *)(&texcoords_[i][j]), sizeof(md3_texcoord));
    }

    for (int j = 0, k = 0; j < surfaces_[i].num_frames * surfaces_[i].num_verts;
         ++j, ++k) {
      if (k >= surfaces_[i].num_verts) {
        k = 0;
      }
      vertices_[i][j].u = texcoords_[i][k].st[0];
      vertices_[i][j].v = texcoords_[i][k].st[1];
    }

    fin.seekg(header_.ofs_surfaces + surfaces_[i].ofs_xyznormal);
    for (int j = 0; j < surfaces_[i].num_frames * surfaces_[i].num_verts; ++j) {
      fin.read((char *)(&normals_[i][j]), sizeof(md3_vertex));

      float scale = (1.0f / 64.0f);

      vertices_[i][j].position.x = (float)(normals_[i][j].coord[0]) * scale;
      vertices_[i][j].position.y = (float)(normals_[i][j].coord[1]) * scale;
      vertices_[i][j].position.z = (float)(normals_[i][j].coord[2]) * scale;

      // extracting normals with code from q3source :)
      float lat;
      float lng;
      lat = (normals_[i][j].normal >> 8) & 0xff;
      lng = (normals_[i][j].normal & 0xff);
      lat *= Q_PI / 128;
      lng *= Q_PI / 128;

      vertices_[i][j].normal.x = cos(lat) * sin(lng);
      vertices_[i][j].normal.y = sin(lat) * sin(lng);
      vertices_[i][j].normal.z = cos(lng);
    }

    fin.seekg(header_.ofs_surfaces + surfaces_[i].ofs_end);
    header_.ofs_surfaces = header_.ofs_surfaces + surfaces_[i].ofs_end;
  }

  glGenBuffers(1, &vboId_);
  glBindBuffer(GL_ARRAY_BUFFER, vboId_);

  glBufferData(GL_ARRAY_BUFFER,
               (surfaces_[0].num_verts) * (surfaces_[0].num_frames) *
                   sizeof(my_vertex),
               NULL, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0,
                  (surfaces_[0].num_verts) * (surfaces_[0].num_frames) *
                      sizeof(my_vertex),
                  vertices_[0]);

  glGenBuffers(1, &iboId_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               (surfaces_[0].num_triangles) * sizeof(md3_triangle), NULL,
               GL_STATIC_DRAW);

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                  (surfaces_[0].num_triangles) * sizeof(md3_triangle),
                  triangles_[0]);

  // load model animation file
}

PlayerModel::PlayerModel(std::string path, std::string name)
    : path_{path}, name_{name} {
  LoadAnimations();
}

void PlayerModel::LoadAnimations() {
  // sex	m
  // headoffset	0 0 0
  // footsteps	normal
  // first frame, num frames, looping frames, frames per second
  // 0	30	0	25		// BOTH_DEATH1

  // TODO: file not found handling
  auto file_name = path_ + name_ + "/animation.cfg";
  auto file_size = boost::filesystem::file_size(file_name);

  char *buffer = new char[file_size + 1];
  std::ifstream fin(file_name);
  fin.read(buffer, file_size);
  buffer[file_size] = 0;

  ParseAnimations(buffer);

  delete[] buffer;
}

/*
unsigned int GetNewLinePosition(const char *buffer, unsigned int offset) {
  while (1) {
    switch (buffer[offset]) {
    case 0x0A:
      return offset;
    case 0x0D:
      return offset;
    }
    ++offset;
  }
}

unsigned int GetTokenEndPosition(const char *buffer, unsigned int offset) {
  while (1) // while buffer[offset] != nullptr
  {
    switch (buffer[offset]) {
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
*/

void SkipLine(char *&buffer) {
  while (1) {
    switch (buffer[0]) {
    case '\n':
      ++buffer;
      return;
    default:
      ++buffer;
    }
  }
}

void SkipWhitespace(char *&buffer) {
  while (1) {
    switch (buffer[0]) {
    case '/':
      if (buffer[1] == '/') {
        SkipLine(buffer);
      }
      break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      ++buffer;
      break;
    default:
      return;
    }
  }
}

void ReadToken(char *token, char *&buffer) {
  SkipWhitespace(buffer);
  token[0] = 0;
  auto i = 0u;
  while (1) {
    switch (buffer[0]) {
    case ' ':
    case '\t':
    case '\n':
    case '\0':
      token[i] = 0;
      return;
    default:
      token[i++] = buffer[0];
      buffer++;
    }
  }
}

void PlayerModel::ParseAnimations(char *buffer) {
  char token[255];
  while (1) {
    ReadToken(token, buffer);
    if (!strcmp(token, "sex")) {
      ReadToken(token, buffer);
      // if m {} else {}
    } else if (!strcmp(token, "headoffset")) {
      ReadToken(token, buffer);
      // x = atoi(token);
      ReadToken(token, buffer);
      // y = atoi(token);
      ReadToken(token, buffer);
      // z = atoi(token);
    } else if (!strcmp(token, "footsteps")) {
      ReadToken(token, buffer);
      // if token == normal
    } else if (token[0] >= '0' && token[0] <= '9') {
      // beginning of animation info
      auto skip = 0u;
      for (auto i = 0u; i < MAX_ANIMATIONS; ++i) {

        if (!token[0]) {
          // parse special cases
          if (i >= TORSO_GETFLAG && i <= TORSO_NEGATIVE) {
            animations_[i].first_frame = animations_[TORSO_GESTURE].first_frame;
            // animations_[i].frame_lerp =
            // animations_[TORSO_GESTURE].frame_lerp;
            // animations_[i].initial_lerp =
            // animations_[TORSO_GESTURE].initial_lerp;
            animations_[i].loop_frames = animations_[TORSO_GESTURE].loop_frames;
            animations_[i].num_frames = animations_[TORSO_GESTURE].num_frames;
            animations_[i].reversed = false;
            continue;
          }
          logger::Debug("Error while loading animation.cfg");
          break;
        }

        animations_[i].first_frame = atoi(token);

        if (i == LEGS_WALKCR) {
          skip = animations_[LEGS_WALKCR].first_frame -
                 animations_[TORSO_GESTURE].first_frame;
        }
        if (i >= LEGS_WALKCR && i < TORSO_GETFLAG) {
          animations_[i].first_frame -= skip;
        }

        ReadToken(token, buffer);
        animations_[i].num_frames = atoi(token);
        ReadToken(token, buffer);
        animations_[i].loop_frames = atoi(token);

        ReadToken(token, buffer);
        int fps = atoi(token);
        if (fps == 0) {
          fps = 1;
        }
        animations_[i].frames_per_second = 1000.0 / fps;

        ReadToken(token, buffer);

        logger::Debug("animation: %d %d %d %f", animations_[i].first_frame,
                      animations_[i].num_frames, animations_[i].loop_frames,
                      animations_[i].frames_per_second);
      }

      memcpy(&animations_[LEGS_BACKCR], &animations_[LEGS_WALKCR],
             sizeof(animation_info));
      animations_[LEGS_BACKCR].reversed = true;
      memcpy(&animations_[LEGS_BACKWALK], &animations_[LEGS_WALK],
             sizeof(animation_info));
      animations_[LEGS_BACKWALK].reversed = true;

      break;
    }
  }
}
