#include "util.h"
#include "bsp.h"
#include "camera.h"
#include "frustum.h"
#include "Logger.h"
#include "Font.h"
#include "Model.h"

#define WIDTH 1280
#define HEIGHT 720

glm::mat4 modelmatrix;
glm::mat4 projectionmatrix;
glm::mat4 orthomatrix;

Frustum g_frustum;
bool g_noclip = true;

Font font;



cmd_t cmds;

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    logger::Log(logger::DEBUG, "Die SDL konnte nicht initialisiert werden (%s)", SDL_GetError());
  }

  IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_WM_SetCaption("Test", "Test2");
  SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_OPENGL);

  // TODO: after initializing glew check if all needed functions are available.. fall back if not or just quit
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    logger::Log(logger::ERROR, "Error: %s", glewGetErrorString(err));
  } 
  logger::Log(logger::DEBUG, "Status: Using GLEW %s", glewGetString(GLEW_VERSION));

  glm::vec4 vEyePt( -10.0f, 10.0f, 20.0f, 1.0f );
  font.LoadFont("gfx\\2d\\bigchars.tga");
  Bsp *map = new Bsp("maps\\q3dm6.bsp");
  //Model model("models\\players\\tankjr\\head.md3");
  //Model model("models\\players\\tankjr\\upper.md3");
  //Model model("models\\players\\tankjr\\lower.md3");
  camera g_cam(vEyePt, map);

  unsigned int ticks = 0;   
  unsigned int delta = 0;

  while (true)
  {
    delta = SDL_GetTicks() - ticks;
    ticks = SDL_GetTicks(); 
          

  }

  IMG_Quit();
  SDL_Quit();
  return 0;
}
