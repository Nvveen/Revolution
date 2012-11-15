#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <SDL/SDL.h>
#include <CEGUI.h>

class Screen {
  public:
    Screen () {}
    Screen (const unsigned int & width, const unsigned int & height);
    virtual ~Screen () {}

    void render();
    void clear();
    bool isOpened();
  protected:
  private:
    unsigned int _w, _h;
    bool _alive;
    SDL_Surface *_surface;
    CEGUI::WindowManager *_winManager;
    double lastTimePulse;

    SDL_Surface *initSDL();
    CEGUI::WindowManager *initCEGUI();
    void setCEGUIPaths();
    void createGUI();
    void injectInput();
    void injectTimePulse(double & ltp);
    void handleMouseDown(Uint8 button);
    void handleMouseUp(Uint8 button);

    static std::string getPath();
};

inline bool Screen::isOpened () {
  return _alive;
}

#endif
