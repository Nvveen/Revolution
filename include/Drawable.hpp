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
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"

#include <glm/gtc/type_precision.hpp>

typedef glm::gtc::type_precision::f64vec3 Vec3;
typedef glm::ivec3 IVec3;

struct Polygon;

class Drawable {
  public:
    Drawable ();
    Drawable ( Shader * const shader, glm::vec4 const & primary );
    virtual ~Drawable ();

    void addPolygon ( std::vector<Vec3> const & vertData,
                      std::vector<IVec3> const & indData );

    void draw ( Camera * const cam );

    std::string name;
  protected:
  private:
    Shader *_shader;
    std::vector<Polygon> _polygons;
    unsigned long _numTriangles;
    glm::vec4 _primaryColor;

    GLuint _vao, _vbo, _ibo;
    glm::mat4 _model;
};

class Polygon {
  public:
    Polygon ();
    virtual ~Polygon() {}

    void addData ( std::vector<Vec3> const & vertData,
                   std::vector<IVec3> const & indData );
    void draw ();
  private:
    std::vector<Vec3> vertices;
    std::vector<IVec3> indices;

    GLuint _vao, _vbo, _ibo;
};

#endif
