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
    
  TTF_Init();

  TTF_Font *font;
  font = TTF_OpenFont("DejaVuSans.ttf", 16);

  std::cout << "Willkommen zum SDL Testprogramm." << std::endl;

  SDL_WM_SetCaption("Test", "Test2");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

  vec3d vEyePt( 0.0, 0.0, 0.0 );
  vec3d vLookatPt( 0.0, 0.0, 10.0 );
  vec3d vUpVec( 0.0, 1.0, 0.0 );
  camera g_cam(&vEyePt, &vLookatPt, &vUpVec);

  glEnable(GL_DEPTH_TEST);
  
  bsp *map = new bsp("maps/q3dm1.bsp");

  while (true)
  {
    g_cam.updateTime(SDL_GetTicks());
    
    SDL_Event event;
        
    while (SDL_PollEvent(&event)) 
    {
      switch (event.type)
      {
      case SDL_QUIT:
        SDL_Quit();
        exit(0);
        break;
      case SDL_MOUSEMOTION:
        g_cam.rotate(event.motion.xrel, event.motion.yrel);
        break;
      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_LEFT)
          g_cam.strafe(-2.0f);
        if(event.key.keysym.sym == SDLK_RIGHT)
          g_cam.strafe(2.0f);
        if(event.key.keysym.sym == SDLK_UP)
          g_cam.move(2.0f);
        if(event.key.keysym.sym == SDLK_DOWN)
          g_cam.move(2.0f);
        break;
      default:
        break;
      }
    }

    glViewport(0, 0, 800, 600);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)800/600, .1, 10000);

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer
    glLoadIdentity(); // Reset orientation
    gluLookAt(g_cam.m_position.x(), g_cam.m_position.y(), g_cam.m_position.z(), 
              g_cam.m_lookat.x(), g_cam.m_lookat.y(), g_cam.m_lookat.z(), 
              g_cam.m_up.x(), g_cam.m_up.y(), g_cam.m_up.z());
        
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Graphical commands go here
    map->render(g_cam.m_position);

    SDL_GL_SwapBuffers(); // Update screen

  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}
