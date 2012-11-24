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
    float _mouseRef[2];
    bool _lookAround;

    SDL_Surface *initSDL();
    CEGUI::WindowManager *initCEGUI();
    void setCEGUIPaths();
    void createGUI();
    void injectInput();
    void injectTimePulse( double & ltp );
    void handleMouseDown( Uint8 button, const int & x, const int & y );
    void handleMouseUp( Uint8 button );

    static std::string getPath();

    void executeInput( const SDLKey & key );
};

inline bool Screen::isOpened() {
  return _alive;
}

#endif
