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
#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <GL/gl.h>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Drawable.hpp"

class World {
  public:
    World () {}
    World (const unsigned int & width, const unsigned int & height);
    virtual ~World();

    void draw ();
    Camera & getCamera();
  protected:
  private:
    Shader *_shader;
    Camera *_cam;
    std::vector<Drawable *> _drawables;
    
    void init ();
};

inline Camera & World::getCamera ()
{
  return *(this->_cam);
}

#endif
