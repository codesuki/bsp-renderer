#include "util.h"
#include "Logger.h"
#include "messenger.h"
#include "input.h"
#include "Renderer.h"
#include "World.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "PlayerInputComponent.h"
#include "PlayerPhysicsComponent.h"

Renderer renderer;
World world;

cmd_t g_cmds;

bool g_running = true;

void QuitCallback(Message* msg)
{
  g_running = false;
}

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    logger::Log(logger::DEBUG, "Could not initialize SDL (%s)", SDL_GetError());
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

  //font.LoadFont("gfx\\2d\\bigchars.tga");
  messenger::RegisterReceiver(MESSAGE::QUIT, QuitCallback);

  input::Initialize();

  renderer.Initialize();

  logger::Log(logger::DEBUG, "Loading shader files");
  shaderLoader::LoadAllShaders();

  world.LoadLevel("q3dm6");

  //Model model("tankjr");
  //Model model("models\\players\\tankjr\\head.md3");
  //Model model("models\\players\\tankjr\\upper.md3");
  //Model model("models\\players\\tankjr\\lower.md3");

  unsigned int ticks = 0;   
  unsigned int delta = 0;


  Entity player;
  PlayerInputComponent pic(player);
  PlayerPhysicsComponent ppc(player);

  player.AddComponent(&pic);
  player.AddComponent(&ppc);

  world.player_ = &player;
  world.players_.push_back(&player);

  player.position_ = glm::vec4( -15.0f, 15.0f, 15.0f, 1.0f );
  renderer.AddRenderables(world.map_->ComputeVisibleFaces(player.position_));

  while (g_running)
  {
    delta = SDL_GetTicks() - ticks;
    ticks = SDL_GetTicks(); 

    g_cmds = input::Update();
    world.Update();

    renderer.RenderFrame((float)ticks/1000);
  }

  shaderLoader::Deinitialize();
  textureLoader::Deinitialize();

  IMG_Quit();
  SDL_Quit();

  return 0;
}
