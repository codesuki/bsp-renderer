#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>

#include "vmmlib/vmmlib.hpp"
using namespace vmml;

//#define GL_GLEXT_PROTOTYPES

#define __USE_VBO__   

#include "glew.h"

#include "SDL.h"
//#include "SDL_opengl.h"
#include "SDL_image.h"

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile);
GLuint CreateProgram(const std::vector<GLuint> &shaderList);

#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define PRINT_VAR(x) std::cout << #x << " " << x << std::endl
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// convert from our coordinate system (looking down X)
// to OpenGL's coordinate system (looking down -Z)     
static const GLfloat quake2oglMatrix[16] = {0, 0, -1, 0,
  -1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 1};

#endif
