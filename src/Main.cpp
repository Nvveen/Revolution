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
