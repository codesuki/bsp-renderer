#include "util.h"
#include "bsp.h"
#include "camera.h"

// convert from our coordinate system (looking down X)
// to OpenGL's coordinate system (looking down -Z)
GLfloat quake2oglMatrix[16] = {0, 0, -1, 0,
                               -1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, 0, 1};

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

  vec3f vEyePt( 0.0, 0.0, -50.0 );
  camera g_cam(&vEyePt);

  glEnable(GL_DEPTH_TEST);
  
  bsp *map = new bsp("maps/q3dm6.bsp");
  unsigned int ticks = 0;
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
      case SDL_MOUSEMOTION:
        g_cam.pitch((float)event.motion.yrel/100.0);
        g_cam.yaw((float)event.motion.xrel/100.0);
        break;
      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_LEFT)
          g_cam.strafe(-20.0f);
        if(event.key.keysym.sym == SDLK_RIGHT)
          g_cam.strafe(20.0f);
        if(event.key.keysym.sym == SDLK_UP)
          g_cam.move(20.0f);
        if(event.key.keysym.sym == SDLK_DOWN)
          g_cam.move(-20.0f);
        break;
      default:
        break;
      }
    }

    glViewport(0, 0, 800, 600);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear color and depth buffer

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (float)800/600, 1, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Reset orientation
    //glLoadMatrixf(quake2oglMatrix);
    //glMultMatrixf(quake2oglMatrix);
    /* gluLookAt(g_cam.m_position.x(), g_cam.m_position.y(), g_cam.m_position.z(), 
              g_cam.m_lookat.x(), g_cam.m_lookat.y(), g_cam.m_lookat.z(), 
              g_cam.m_up.x(), g_cam.m_up.y(), g_cam.m_up.z());
    */

    glLoadMatrixf(g_cam.GetMatrix());
  
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Graphical commands go here
    map->render(g_cam.position_);

    SDL_GL_SwapBuffers(); // Update screen

  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}
