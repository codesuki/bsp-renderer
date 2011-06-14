#include "util.h"
#include "bsp.h"
#include "camera.h"

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    std::cout << "Die SDL konnte nicht initialisiert werden (" 
      << SDL_GetError() << ")" << std::endl;
  }

  IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_WM_SetCaption("Test", "Test2");
  SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  } 
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  

  vec3f vEyePt( -10.0, 10.0, 20.0 );
  camera g_cam(&vEyePt);

  SDL_WarpMouse(400, 300);

  bsp *map = new bsp("maps/q3dm6.bsp");
  unsigned int ticks = 0;   

  glEnable(GL_DEPTH_TEST); 
  glDisable(GL_LIGHTING);

#ifndef __USE_SHADERS__
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
#endif

  while (true)
  {
    g_cam.updateTime(SDL_GetTicks() - ticks);
    ticks = SDL_GetTicks(); 

    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
      switch (event.type)
      {
        case SDL_QUIT:
          SDL_Quit();
          exit(0);
          break;
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_ESCAPE)
          {    
            SDL_Quit();
            exit(0);
          }
          break;
        default:
          break;
      }
    }                      

    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (keystate[SDLK_a]) g_cam.strafe(-1.0f);
    if (keystate[SDLK_d]) g_cam.strafe(1.0f);
    if (keystate[SDLK_w]) g_cam.move(-1.0f);
    if (keystate[SDLK_s]) g_cam.move(1.0f);

    int x,y;
    SDL_PumpEvents();
    SDL_GetMouseState(&x, &y);
    SDL_WarpMouse(400, 300);
    float rx = ((((float)x)-400.f)/100.f);
    float ry = ((((float)y)-300.f)/100.f);

    if (rx != 0 || ry != 0)
    {  
      g_cam.pitch(-ry);
      g_cam.yaw(-rx);
    }
    glViewport(0, 0, 800, 600);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (float)800/600, 1, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Reset orientation               x

    glMultMatrixf(g_cam.GetMatrix());
    glMultMatrixf(quake2oglMatrix);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Graphical commands go here
    map->render(g_cam.position_);
    SDL_GL_SwapBuffers();

    unsigned int delta = SDL_GetTicks() - ticks;
    if (delta != 0)	
      std::cout << "fps: " << 1000 / delta << std::endl;  

    //SDL_ShowCursor(SDL_DISABLE);
  }

  IMG_Quit();
  SDL_Quit();
  return 0;
}
