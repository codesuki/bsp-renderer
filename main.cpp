#include "util.h"
#include "Logger.h"
#include "Renderer.h"
#include "World.h"
#include "ShaderLoader.h"

#define WIDTH 1280
#define HEIGHT 720

Renderer renderer;
World world;

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

  //font.LoadFont("gfx\\2d\\bigchars.tga");

  //Bsp *map = new Bsp("maps\\q3dm6.bsp");

  //Model model("tankjr");


  logger::Log(logger::DEBUG, "Loading shader files");
  shaderLoader::LoadAllShaders();

  world.LoadLevel("q3dm6");

  //Model model("models\\players\\tankjr\\head.md3");
  //Model model("models\\players\\tankjr\\upper.md3");
  //Model model("models\\players\\tankjr\\lower.md3");

  unsigned int ticks = 0;   
  unsigned int delta = 0;

  while (true)
  {
    delta = SDL_GetTicks() - ticks;
    ticks = SDL_GetTicks(); 

    world.Update();
    renderer.RenderFrame((float)delta/1000);
  }

  IMG_Quit();
  SDL_Quit();

  return 0;
}
