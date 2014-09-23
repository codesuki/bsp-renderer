#include "input.hpp"

#include <map>

#include <SDL2/SDL.h>

#include "messenger.hpp"
#include "message.hpp"
#include "logger.hpp"

namespace input {
  namespace 
  {
    std::map<unsigned int, MESSAGE> keymap_;
  }

  void Initialize()
  {
    keymap_[SDL_SCANCODE_ESCAPE] = MESSAGE::QUIT; 

    keymap_[SDL_SCANCODE_W] = MESSAGE::CMD_FORWARD;
    keymap_[SDL_SCANCODE_S] = MESSAGE::CMD_BACKWARD;
    keymap_[SDL_SCANCODE_A] = MESSAGE::CMD_LEFT;
    keymap_[SDL_SCANCODE_D] = MESSAGE::CMD_RIGHT;
    keymap_[SDL_SCANCODE_N] = MESSAGE::NOCLIP;

    SDL_SetRelativeMouseMode(SDL_TRUE);
  }

  cmd_t Update() 
  {
    SDL_PumpEvents();
    auto keystate = SDL_GetKeyboardState(NULL);

    for (auto& kv : keymap_) 
    {
      if (keystate[kv.first]) 
      {
        messenger::BroadcastMessage(kv.second, nullptr);
      }
    }

    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    //SDL_WarpMouseGlobal(400, 300);
    float rx = ((((float)x)-400.f)/100.f);
    float ry = ((((float)y)-300.f)/100.f);

    rx = x/100.f;
    ry = y/100.f;

    //messenger::BroadcastMessage(MESSAGE::CMD_MOUSELOOK, &MouseMoveMessage(rx, ry));
    cmd_t cmds;
    memset(&cmds, 0, sizeof(cmds));
    if (keystate[SDL_SCANCODE_W]) cmds.forward_move -= 125;
    if (keystate[SDL_SCANCODE_S]) cmds.forward_move += 125;
    if (keystate[SDL_SCANCODE_D]) cmds.right_move += 125;
    if (keystate[SDL_SCANCODE_A]) cmds.right_move -= 125;
    if (keystate[SDL_SCANCODE_SPACE]) cmds.up_move += 125;
    cmds.mouse_dx = -rx;
    cmds.mouse_dy = -ry;

    return cmds;
  }
}
