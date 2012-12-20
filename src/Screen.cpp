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
#include "Screen.hpp"
#include <iostream>
#ifndef NOCEGUI
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>
#endif
#include <unistd.h>

#ifndef NOCEGUI
std::string CEGUIInstallBasePath;
#endif

Screen::Screen( const unsigned int & width, const unsigned int & height ) :
  _w(width), _h(height), _alive(true) {
#ifndef NOCEGUI
  CEGUIInstallBasePath = getPath() + "/";
#endif
  _surface = initSDL();
#ifndef NOCEGUI
  _winManager = initCEGUI();
#endif
  _lookAround = false;
  createGUI();
  lastTimePulse = 0.001 * static_cast<double>(SDL_GetTicks());
}

Screen::~Screen() {
  std::cout << "Cleaning up the screen..." << std::endl;
  delete _world;
#ifndef NOCEGUI
  delete _winManager;
#endif
  SDL_FreeSurface(_surface);
}

#ifndef NOCEGUI
CEGUI::WindowManager *Screen::initCEGUI() {
  std::cout << " - initializing CEGUI" << std::endl;
  CEGUI::OpenGLRenderer::bootstrapSystem();
  this->setCEGUIPaths();
  CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
  CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook",
      "MouseArrow");
  return &(CEGUI::WindowManager::getSingleton());
}
#endif
 
SDL_Surface *Screen::initSDL() {
  std::cout << "initializing SDL" << std::endl;
  atexit(SDL_Quit);
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Unable to initialise SDL: " << SDL_GetError();
    exit(0);
  }
  SDL_Surface * screen = SDL_SetVideoMode (_w, _h, 0, SDL_OPENGL | SDL_RESIZABLE);
  if (screen == 0) {
    std::cerr << "Unable to set OpenGL videomode: " << SDL_GetError();
    SDL_Quit();
    exit(0);
  }
#ifdef NOCEGUI
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
    exit(1);
  }
#endif
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  // glDisable(GL_CULL_FACE);
  glFrontFace(GL_CCW);

  return screen;
}

void Screen::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
 
void Screen::render() {
  injectInput();
  injectTimePulse(lastTimePulse);
#ifndef NOCEGUI
  // Renders the GUI:
  CEGUI::System::getSingleton().renderGUI();
#endif
  // Render the world
  _world->draw();
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
 
#ifndef NOCEGUI
void Screen::setCEGUIPaths() {
	// Initialises the required directories for the DefaultResourceProvider:
	CEGUI::DefaultResourceProvider & defaultResProvider =
    *static_cast<CEGUI::DefaultResourceProvider*>(
        CEGUI::System::getSingleton().getResourceProvider());
	const std::string CEGUIInstallSharePath = CEGUIInstallBasePath
	  + "share/";
	// For each resource type, sets a corresponding resource group directory:
	std::cout << "Using scheme directory '" << CEGUIInstallSharePath + "schemes/"
		 << "'" << std::endl;
	defaultResProvider.setResourceGroupDirectory("schemes",
	  CEGUIInstallSharePath + "schemes/");
	defaultResProvider.setResourceGroupDirectory("imagesets",
	  CEGUIInstallSharePath + "imagesets/");
	defaultResProvider.setResourceGroupDirectory("fonts",
	  CEGUIInstallSharePath + "fonts/");
	defaultResProvider.setResourceGroupDirectory("layouts",
	  CEGUIInstallSharePath + "layouts/");
	defaultResProvider.setResourceGroupDirectory("looknfeels",
	  CEGUIInstallSharePath + "looknfeel/");
	defaultResProvider.setResourceGroupDirectory("lua_scripts",
	  CEGUIInstallSharePath + "lua_scripts/");
	defaultResProvider.setResourceGroupDirectory("schemas",
	  CEGUIInstallSharePath + "xml_schemas/");
	defaultResProvider.setResourceGroupDirectory("animations",
	  CEGUIInstallSharePath + "animations/");
	// Sets the default resource groups to be used:
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup( "layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
 
	// Set-up default group for validation schemas:
	CEGUI::XMLParser * parser = CEGUI::System::getSingleton().getXMLParser();
	if ( parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}
#endif
 
void Screen::createGUI() {
  std::cout << "Creating the GUI..." << std::endl;
  _world = new World(_w, _h);
}

void Screen::handleMouseDown( Uint8 button, const int & x, const int & y ) {
  switch ( button ) {
#ifndef NOCEGUI
    case SDL_BUTTON_LEFT:
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
      break;
    case SDL_BUTTON_MIDDLE:
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
      break;
#endif
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
      std::cout << "handleMouseDown ignored '" << static_cast<int>(button)
         << "'" << std::endl;
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
    std::cout << "handleMouseUp ignored '" << static_cast<int>(button)
       << "'" << std::endl;
    break;
  }
 
}

std::string Screen::getPath() {
  int max = 256;
  char temp[max];
  std::string str = (getcwd(temp, max) ? std::string(temp) : std::string(""));
  chdir(str.c_str());
  return str;
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
    case SDLK_p:
      _world->showMore();
      break;
    case SDLK_o:
      _world->showLess();
      break;
    case SDLK_k:
    case SDLK_l:
    case SDLK_m:
    case SDLK_n:
      static float diffInt = _world->getDefaultLight().DiffuseIntensity;
      static float ambInt = _world->getDefaultLight().AmbientIntensity;
      if (key == SDLK_m)
        diffInt += 0.05f;
      if (key == SDLK_n)
        diffInt -= 0.05f;
      if (key == SDLK_l)
        ambInt += 0.05f;
      if (key == SDLK_k)
        ambInt -= 0.05f;
      if (diffInt > 1.0f) 
        diffInt = 1.0f;
      if (diffInt < 0.0f)
        diffInt = 0.0f;
      if (ambInt > 1.0f)
        ambInt = 1.0f;
      if (ambInt < 0.0f)
        ambInt = 0.0f;
      if (key == SDLK_n || key == SDLK_m)
        _world->changeDiffuseIntensity(diffInt);
      else
        _world->changeAmbientIntensity(ambInt);
      std::cout << ambInt << " " << diffInt << std::endl;
      break;
    default:
      break;
  }
}
