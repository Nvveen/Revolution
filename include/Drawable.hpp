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
#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <vector>
#include "Shader.hpp"
#include "Camera.hpp"

class Drawable {
  public:
    Drawable ();
    Drawable ( Shader * const shader, glm::vec4 const & primary );
    virtual ~Drawable ();

    void draw ( Camera * const cam );
  protected:
  private:
    Shader *_shader;
    std::vector<GLfloat> _data;
    glm::vec4 _primaryColor;

    GLuint _vao, _vbo;
    glm::mat4 _model;

    void init ();
};

#endif
