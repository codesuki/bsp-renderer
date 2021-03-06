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

#include "GL/glew.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GL_GLEXT_PROTOTYPES

#include "SDL.h"
#include "SDL_image.h"

#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define PRINT_VAR(x) std::cout << #x << " " << x << std::endl
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define glError() { \
  GLenum err = glGetError(); \
  while (err != GL_NO_ERROR) { \
  fprintf(stderr, "glError: %s caught at %s:%u\n", (char *)gluErrorString(err), __FILE__, __LINE__); \
  err = glGetError(); \
  } \
}

// convert from our coordinate system (looking down X)
// to OpenGL's coordinate system (looking down -Z)     
static const GLfloat quake2oglMatrix[16] = 
{0, 0, -1, 0,
-1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 0, 1};

//static const glm::mat4 quake2ogl(0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);


#endif
