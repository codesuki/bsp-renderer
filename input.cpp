#include "Input.h"

#include <map>

#include "util.h"

#include "messenger.h"
#include "Message.h"

namespace input {
  namespace 
  {
    std::map<unsigned int, MESSAGE> keymap_;
  }

  void Initialize()
  {
    keymap_[SDLK_ESCAPE] = MESSAGE::QUIT; 

    keymap_[SDLK_w] = MESSAGE::CMD_FORWARD;
    keymap_[SDLK_s] = MESSAGE::CMD_BACKWARD;
    keymap_[SDLK_a] = MESSAGE::CMD_LEFT;
    keymap_[SDLK_d] = MESSAGE::CMD_RIGHT;

    SDL_WarpMouse(400, 300);
    SDL_ShowCursor(SDL_DISABLE);
  }

  cmd_t Update() 
  {
    SDL_PumpEvents();
    Uint8 *keystate = SDL_GetKeyState(NULL);

    for (auto& kv : keymap_) 
    {
      if (keystate[kv.first]) 
      {
        messenger::BroadcastMessage(kv.second, nullptr);
      }
    }

    int x, y;
    SDL_GetMouseState(&x, &y);
    SDL_WarpMouse(400, 300);
    float rx = ((((float)x)-400.f)/100.f);
    float ry = ((((float)y)-300.f)/100.f);

    //messenger::BroadcastMessage(MESSAGE::CMD_MOUSELOOK, &MouseMoveMessage(rx, ry));
    cmd_t cmds;
    memset(&cmds, 0, sizeof(cmds));
    if (keystate[SDLK_w]) cmds.forward_move += 125;
    if (keystate[SDLK_s]) cmds.forward_move -= 125;
    if (keystate[SDLK_d]) cmds.right_move += 125;
    if (keystate[SDLK_a]) cmds.right_move -= 125;
    cmds.mouse_dx = rx;
    cmds.mouse_dy = ry;

    return cmds;
  }
}