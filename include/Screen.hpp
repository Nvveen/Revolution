#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <CEGUI.h>
#include "World.hpp"

class Screen {
  public:
    Screen () {}
    Screen( const unsigned int & width, const unsigned int & height );
    virtual ~Screen ();

    void render();
    void clear();
    bool isOpened();
  protected:
  private:
    unsigned int _w, _h;
    bool _alive;
    double lastTimePulse;
    SDL_Surface *_surface;
    CEGUI::WindowManager *_winManager;
    World *_world;

    SDL_Surface *initSDL();
    CEGUI::WindowManager *initCEGUI();
    void setCEGUIPaths();
    void createGUI();
    void injectInput();
    void injectTimePulse( double & ltp );
    void handleMouseDown( Uint8 button );
    void handleMouseUp( Uint8 button );

    static std::string getPath();
};

inline bool Screen::isOpened() {
  return _alive;
}

#endif
