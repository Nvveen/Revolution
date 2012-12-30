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
#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <GL/glew.h>
#include <SDL/SDL.h>
#ifndef NOCEGUI
#include <CEGUI/CEGUI.h>
#endif
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
    World *_world;
    float _mouseRef[2];
    bool _lookAround;

    SDL_Surface *initSDL();
    void setCEGUIPaths();
    void createGUI();
    void injectInput();
    void injectTimePulse( double & ltp );
    void handleMouseDown( Uint8 button, const int & x, const int & y );
    void handleMouseUp( Uint8 button );

    static std::string getPath();

    void executeInput( const SDLKey & key );

    // CEGUI stuff
#ifndef NOCEGUI
    void initCEGUI();
#endif
};

inline bool Screen::isOpened() {
  return _alive;
}

class ComboxTextItem : public CEGUI::ListboxTextItem {
  public:
    ComboxTextItem(CEGUI::String const & str) : CEGUI::ListboxTextItem(str) {
      setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
      setTextColours(0xFFFFFFFF);
    }
};

#endif
