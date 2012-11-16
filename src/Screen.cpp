#include "Screen.hpp"
#include <iostream>
#include <RendererModules/OpenGL/CEGUIOpenGLRenderer.h>

std::string CEGUIInstallBasePath;

Screen::Screen( const unsigned int & width, const unsigned int & height ) :
  _w(width), _h(height), _alive(true) {
  CEGUIInstallBasePath = getPath() + "/";
  _surface = initSDL();
  _winManager = initCEGUI();
  createGUI();
  lastTimePulse = 0.001 * static_cast<double>(SDL_GetTicks());
}

CEGUI::WindowManager *Screen::initCEGUI() {
  std::cout << " - initializing CEGUI" << std::endl;
  CEGUI::OpenGLRenderer::bootstrapSystem();
  this->setCEGUIPaths();
  CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
  CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook",
      "MouseArrow");
  return &(CEGUI::WindowManager::getSingleton());
}
 
SDL_Surface *Screen::initSDL() {
  std::cout << " - initializing SDL" << std::endl;
  atexit(SDL_Quit);
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Unable to initialise SDL: " << SDL_GetError();
    exit(0);
  }
  SDL_Surface * screen = SDL_SetVideoMode (_w, _h, 0, SDL_OPENGL);
  if (screen == 0) {
    std::cerr << "Unable to set OpenGL videomode: " << SDL_GetError();
    SDL_Quit();
    exit(0);
  }
  SDL_ShowCursor(SDL_DISABLE);
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  return screen;
}

void Screen::clear() {
  glClear(GL_COLOR_BUFFER_BIT);
}
 
void Screen::render() {
  injectInput();
  injectTimePulse(lastTimePulse);
  // Renders the GUI:
  CEGUI::System::getSingleton().renderGUI();
  // Updates the screen:
  SDL_GL_SwapBuffers();
}
 
void Screen::injectInput() {
  SDL_Event e;
  // Go through all available events:
  while (SDL_PollEvent(&e)) {
    // Route according to the event type:
    switch (e.type) {
      // Mouse section:
      case SDL_MOUSEMOTION:
        // We inject the mouse position directly here:
        CEGUI::System::getSingleton().injectMousePosition(static_cast<float>(
              e.motion.x), static_cast<float>(e.motion.y));
        break;
      case SDL_MOUSEBUTTONDOWN:
        handleMouseDown(e.button.button);
        break;
      case SDL_MOUSEBUTTONUP:
        handleMouseUp(e.button.button);
        break;
      // Keyboard section:
      case SDL_KEYDOWN:
        CEGUI::System::getSingleton().injectKeyDown(e.key.keysym.scancode);
        /*
         * Managing the character is more difficult, we have to use a translated
         * unicode value:
         *
         */
        if ((e.key.keysym.unicode & 0xFF80) == 0) {
          CEGUI::System::getSingleton().injectChar(e.key.keysym.unicode & 0x7F);
        }
        break;
      case SDL_KEYUP:
        CEGUI::System::getSingleton().injectKeyUp( e.key.keysym.scancode );
        break;
      // A WM quit event occured:
      case SDL_QUIT:
        _alive = false;
        break;
      case SDL_VIDEORESIZE:
        CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(
              e.resize.w, e.resize.h));
        break;
    }
  }
}

void Screen::injectTimePulse( double & ltp ) {
  double current = 0.001 * SDL_GetTicks();
  CEGUI::System::getSingleton().injectTimePulse(static_cast<float>(current -
        ltp));
  ltp = current;
}
 
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
 
void Screen::createGUI() {
  // Hier komt de eigen UI in
  // std::cout << " - creating the GUI" << std::endl;
  // CEGUI::DefaultWindow & rootWin = *static_cast<CEGUI::DefaultWindow*>(
  //     _winManager->createWindow("DefaultWindow", "Root"));
  // CEGUI::System::getSingleton().setGUISheet(&rootWin);
  // CEGUI::FrameWindow & myWin = *static_cast<CEGUI::FrameWindow*>(
  //     _winManager->createWindow("TaharezLook/FrameWindow", "Demo Window"));
  // rootWin.addChildWindow(&myWin);
  // myWin.setPosition(CEGUI::UVector2( cegui_reldim(0.25f), cegui_reldim(0.25f)));
  // myWin.setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.5f)));
  // myWin.setMaxSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim(1.0f)));
  // myWin.setMinSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim(0.1f)));
  // myWin.setText("Hello World! This is a minimal SDL+OpenGL+CEGUI test.");
}

void Screen::handleMouseDown( Uint8 button ) {
  switch ( button ) {
    case SDL_BUTTON_LEFT:
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
      break;
    case SDL_BUTTON_MIDDLE:
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
      break;
    case SDL_BUTTON_RIGHT:
      CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
      break;
    case SDL_BUTTON_WHEELDOWN:
      CEGUI::System::getSingleton().injectMouseWheelChange(-1);
      break;
    case SDL_BUTTON_WHEELUP:
      CEGUI::System::getSingleton().injectMouseWheelChange(+1);
      break;
    default:
      std::cout << "handleMouseDown ignored '" << static_cast<int>(button)
         << "'" << std::endl;
      break;
  }
}
 
void Screen::handleMouseUp( Uint8 button ) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
      break;
    case SDL_BUTTON_MIDDLE:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
      break;
    case SDL_BUTTON_RIGHT:
      CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
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
  return (getcwd(temp, max) ? std::string(temp) : std::string(""));
}
