// This file is part of Revolution.
// 
// Revolution is free software: you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the 
// Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// Revolution is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with 
// Revolution. If not, see <http://www.gnu.org/licenses/>.
#include <GL/glew.h>
#include "Screen.hpp"


Screen::Screen( const unsigned int & width, const unsigned int & height ) :
  _w(width), _h(height), _alive(true) {
  _surface = initSDL();
  createGUI();
  _lookAround = false;
  lastTimePulse = 0.001 * static_cast<double>(SDL_GetTicks());
}

Screen::~Screen() {
  std::cout << "Cleaning up the screen..." << std::endl;
  delete _world;
  delete _gui;
  SDL_FreeSurface(_surface);
}
 
SDL_Surface *Screen::initSDL() {
  std::cout << "initializing SDL" << std::endl;
  atexit(SDL_Quit);
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Unable to initialise SDL: " << SDL_GetError();
    exit(0);
  }
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Surface * screen = SDL_SetVideoMode (_w, _h, 0, SDL_OPENGL | SDL_RESIZABLE);
  if (screen == 0) {
    std::cerr << "Unable to set OpenGL videomode: " << SDL_GetError();
    SDL_Quit();
    exit(0);
  }
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
    exit(1);
  }
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_ShowCursor(SDL_DISABLE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_FOG);
  glClearColor(0, 0, 0.3, 1);
  glClearStencil(0);
  glFrontFace(GL_CCW);

  return screen;
}
 
void Screen::createGUI() {
  std::cout << "Creating the GUI..." << std::endl;
  try {
    World::initWorld(_w, _h);
    _world = &World::getSingleton();
    _gui = new GUIManager;
    _gui->populateDatalists(_world->getDatasets());
  }
  catch (GUIManagerException & e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  catch (...) {
  }
}

void Screen::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
 
void Screen::render() {
  // Render the world
  _world->draw();
  // Get input
  injectInput();
  injectTimePulse(lastTimePulse);
  _gui->render();
  // Updates the screen:
  SDL_GL_SwapBuffers();
}
 
void Screen::injectInput ()
{
  SDL_Event e;
  // Go through all available events:
  while (SDL_PollEvent(&e)) {
    // Route according to the event type:
    switch (e.type) {
      // Mouse section:
      case SDL_MOUSEMOTION:
#ifndef NOCEGUI
        // We inject the mouse position directly here:
        CEGUI::System::getSingleton().injectMousePosition(static_cast<float>(
              e.motion.x), static_cast<float>(e.motion.y));
#endif
        if (_lookAround) {
          _world->getCamera().look(_mouseRef[0], _mouseRef[1], e.motion.x,
              e.motion.y);
          SDL_WarpMouse((unsigned short)(_mouseRef[0]),
                        (unsigned short)(_mouseRef[1]));
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        handleMouseDown(e.button.button, e.button.x, e.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        handleMouseUp(e.button.button);
        break;
      // Keyboard section:
      case SDL_KEYDOWN:
#ifndef NOCEGUI
        CEGUI::System::getSingleton().injectKeyDown(e.key.keysym.scancode);
#endif
        /*
         * Managing the character is more difficult, we have to use a translated
         * unicode value:
         *
         */
        if ((e.key.keysym.unicode & 0xFF80) == 0) {
#ifndef NOCEGUI
          CEGUI::System::getSingleton().injectChar(e.key.keysym.unicode & 0x7F);
#endif
          executeInput(e.key.keysym.sym);
        }
        break;
      case SDL_KEYUP:
#ifndef NOCEGUI
        CEGUI::System::getSingleton().injectKeyUp( e.key.keysym.scancode );
#endif
        break;
      // A WM quit event occured:
      case SDL_QUIT:
        _alive = false;
        break;
      case SDL_VIDEORESIZE:
#ifndef NOCEGUI
        CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(
              e.resize.w, e.resize.h));
#endif
        // TODO camera resize here.
        break;
    }
  }
}

void Screen::injectTimePulse ( double & ltp )
{
  double current = 0.001 * SDL_GetTicks();
#ifndef NOCEGUI
  CEGUI::System::getSingleton().injectTimePulse(static_cast<float>(current -
        ltp));
#endif
  ltp = current;
}

void Screen::handleMouseDown( Uint8 button, const int & x, const int & y ) {
  switch ( button ) {
    case SDL_BUTTON_LEFT:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
#endif
      _world->selectCountry(x, _h-y-1);
      break;
    case SDL_BUTTON_MIDDLE:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
#endif
      break;
    case SDL_BUTTON_RIGHT:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
#endif
      _lookAround = true;
      _mouseRef[0] = float(x); _mouseRef[1] = float(y);
      SDL_ShowCursor(SDL_DISABLE);
      break;
    case SDL_BUTTON_WHEELDOWN:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseWheelChange(-1);
#endif
      break;
    case SDL_BUTTON_WHEELUP:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseWheelChange(+1);
#endif
      break;
    default:
      // std::cout << "handleMouseDown ignored '" << static_cast<int>(button)
      //    << "'" << std::endl;
      break;
  }
}
 
void Screen::handleMouseUp( Uint8 button ) {
  switch (button) {
#ifndef NOCEGUI
    case SDL_BUTTON_LEFT:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      break;
    case SDL_BUTTON_MIDDLE:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      break;
#endif
    case SDL_BUTTON_RIGHT:
#ifndef NOCEGUI
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
#endif
      _lookAround = false;
      SDL_ShowCursor(SDL_ENABLE);
      break;
    case SDL_BUTTON_WHEELDOWN:
      break;
    case SDL_BUTTON_WHEELUP:
      break;
    default:
    // std::cout << "handleMouseUp ignored '" << static_cast<int>(button)
    //    << "'" << std::endl;
    break;
  }
 
}

void Screen::executeInput( const SDLKey & key ) {
  switch (key) {
    case SDLK_UP:
    case SDLK_w:
      _world->getCamera().move(0.0f, 0.0f, 3.0f);
      break;
    case SDLK_DOWN:
    case SDLK_s:
      _world->getCamera().move(0.0f, 0.0f, -3.0f);
      break;
    case SDLK_LEFT:
    case SDLK_a:
      _world->getCamera().move(3.0f, 0.0f, 0.0f);
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      _world->getCamera().move(-3.0f, 0.0f, 0.0f);
      break;
    case SDLK_z:
      _world->getCamera().move(0.0f, 3.0f, 0.0f);
      break;
    case SDLK_x:
      _world->getCamera().move(0.0f, -3.0f, 0.0f);
      break;
    default:
      break;
  }
}
