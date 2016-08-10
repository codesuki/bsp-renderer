#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "bsp.hpp"
#include "input.hpp"
#include "logger.hpp"
#include "messenger.hpp"
#include "model.hpp"
#include "player_animation_component.hpp"
#include "player_input_component.hpp"
#include "player_physics_component.hpp"
#include "renderer.hpp"
#include "shader_loader.hpp"
#include "texture_loader.hpp"
#include "world.hpp"

Renderer renderer;
World world;

Model *head;
Model *upper;
Model *lower;

Entity player;
PlayerInputComponent pic1;
PlayerPhysicsComponent ppc1;

Entity enemy;
PlayerAnimationComponent pac2;
PlayerPhysicsComponent ppc2;

bool is_player = true;

cmd_t g_cmds;

bool g_running = true;

void QuitCallback(Message *msg) { g_running = false; }

void NoclipCallback(Message *msg) {
  logger::Log(logger::DEBUG, "NOCLIP SWITCH %d", !world.player_->noclip_);
  world.player_->noclip_ = !world.player_->noclip_;
}

void ChangePlayerCallback(Message *msg) {
  logger::Log(logger::DEBUG, "Switching players %d", !is_player);
  is_player = !is_player;

  if (is_player) {
    ppc1.set_active(true);
    ppc2.set_active(false);
  } else {
    ppc1.set_active(false);
    ppc2.set_active(true);
  }
}

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    logger::Log(logger::DEBUG, "Could not initialize SDL (%s)", SDL_GetError());
  }

  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  auto screen =
      SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

  auto gl_context = SDL_GL_CreateContext(screen);

  // TODO: after initializing glew check if all needed functions are
  // available...
  // fall back if not or just quit
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    logger::Log(logger::ERROR, "Error: %s", glewGetErrorString(err));
  }
  logger::Log(logger::ERROR, "Status: Using GLEW %s",
              glewGetString(GLEW_VERSION));

  // font.LoadFont("gfx\\2d\\bigchars.tga");
  messenger::RegisterReceiver(MESSAGE::QUIT, QuitCallback);
  messenger::RegisterReceiver(MESSAGE::NOCLIP, NoclipCallback);
  messenger::RegisterReceiver(MESSAGE::NEXT_PLAYER, ChangePlayerCallback);

  input::Initialize();

  renderer.Initialize();

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  logger::Log(logger::DEBUG, "Loading shader files");
  shaderLoader::LoadAllShaders();

  world.LoadLevel("q3dm6");

  // PlayerModel *m = new PlayerModel("models/players/", "visor");

  // Model model("tankjr");
  head = new Model("models/players/visor/head.md3");
  head->shader_ =
      shaderLoader::CreateModelShader("models/players/visor/red.tga");
  upper = new Model("models/players/visor/upper.md3");
  upper->shader_ =
      shaderLoader::CreateModelShader("models/players/visor/red.tga");
  lower = new Model("models/players/visor/lower.md3");
  lower->shader_ =
      shaderLoader::CreateModelShader("models/players/visor/red.tga");

  // load weapon

  unsigned int ticks = 0;
  unsigned int delta = 0;

  player.AddComponent(&pic1);
  player.AddComponent(&ppc1);
  player.noclip_ = true;

  world.player_ = &player;
  world.players_.push_back(&player);

  enemy.lower = lower;
  enemy.upper = upper;
  enemy.head = head;
  enemy.noclip_ = false;

  enemy.AddComponent(&pac2);
  enemy.AddComponent(&ppc2);

  {
    int skip = 153 - 90;
    int g_lower_startFrame = 220 - skip;

    enemy.lower_frame = g_lower_startFrame;
    enemy.upper_frame = 90;
  }

  world.enemy_ = &enemy;
  world.players_.push_back(&enemy);

  player.position_ = glm::vec4(-589.0f, -275.0f, 128.0f, 1.0f);
  // enemy.position_ = glm::vec4(-589.0f, -275.0f, 100.0f, 1.0f); // center room
  enemy.position_ = glm::vec4(251.8f, -816.1f, 30.0f, 1.0f); // staircase

  while (g_running) {
    if (SDL_QuitRequested()) {
      g_running = false;
    }

    delta = SDL_GetTicks() - ticks;
    ticks = SDL_GetTicks();

    g_cmds = input::Update();
    world.Update(delta);

    renderer.AddRenderables(world.map_->ComputeVisibleFaces(player.position_));
    renderer.RenderFrame((float)ticks / 1000.0f);

    SDL_GL_SwapWindow(screen);
  }

  shaderLoader::Deinitialize();
  textureLoader::Deinitialize();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(screen);

  IMG_Quit();
  SDL_Quit();

  return 0;
}
