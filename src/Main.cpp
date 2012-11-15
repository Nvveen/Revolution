#include <iostream>
#include "Screen.hpp"

const unsigned int width = 1024, height = 768;

void loop (Screen & sc) {
  std::cout << " - entering main loop" << std::endl;
  while (sc.isOpened()) {
    sc.clear();
    sc.render();
  }
  std::cout << " - leaving main loop" << std::endl;
}
 
int main( int argc, char *argv[] ) {
  std::cout << " - starting CEGUI" << std::endl;
  Screen s(width, height);
  loop(s);
  std::cout << " - ending CEGUI" << std::endl;
 
}
