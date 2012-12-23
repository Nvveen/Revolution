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

typedef glm::ivec3 IVec3;

struct Polygon;

struct Vertex {
  glm::dvec3 position;
  glm::dvec3 normal = glm::dvec3(0.0, 0.0, 0.0);
};

class Drawable {
  public:
    Drawable ();
    Drawable ( Shader * const shader, glm::vec4 const & primary );
    virtual ~Drawable ();

    void addPolygon ( std::vector<Vertex> const & vertData,
                      std::vector<IVec3> const & indData );
    void draw ( Camera * const cam );
    void setHeightDistortion ( float const & height );

    std::string name;
  protected:
  private:
    Shader *_shader;
    std::vector<Polygon> _polygons;
    unsigned long _numTriangles;
    glm::vec4 _primaryColor;

    glm::mat4 _model;
    float _heightDistortion;
};

class Polygon {
  public:
    Polygon ();
    virtual ~Polygon() {}

    void addData ( std::vector<Vertex> const & vertData,
                   std::vector<IVec3> const & indData );
    void draw ();

    friend class Drawable;
  private:
    std::vector<Vertex> _vertices;
    std::vector<IVec3> _triangles;

    GLuint _vao, _vbo, _ibo;
};

inline void Drawable::setHeightDistortion ( float const & height )
{
  _heightDistortion = height;
  if (_heightDistortion > 30) _heightDistortion = 30;
  else if (_heightDistortion < 0) _heightDistortion = 0;
}

#endif
