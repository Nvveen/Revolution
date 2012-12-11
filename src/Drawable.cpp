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
#include <GL/glew.h>
#include "Drawable.hpp"

Drawable::Drawable ()
{
}

Drawable::Drawable ( Shader * const shader, glm::vec4 const & primary ) : 
  _shader(shader), _primaryColor(primary), _model(glm::mat4(1.0))
{
  this->init();
}

Drawable::~Drawable ()
{
}

void Drawable::init ()
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ibo);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Drawable::draw ( Camera * const cam )
{
  _shader->bind();
  glm::mat4 mvp = cam->transform(_model);
  _shader->setUniform("vMVP", mvp);
  _shader->setUniform("objectColor", _primaryColor);

  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glDrawElements(GL_TRIANGLES, _numTriangles, GL_UNSIGNED_INT, (void *)0);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  _shader->unbind();
}

void Drawable::addPolygons ( std::vector<Polygon> const & polygons )
{
  // Determine size of buffers
  unsigned int vboSize = 0, iboSize = 0;
  for ( auto poly : polygons ) {
    vboSize += poly.vertices.size();
    iboSize += poly.indices.size();
  }
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  // Prepare buffers
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLdouble)*vboSize*3, NULL,
               GL_DYNAMIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*iboSize*3, NULL,
               GL_DYNAMIC_DRAW);
  // Write data to buffers in parts
  unsigned int vboOffset = 0, iboOffset = 0;
  _numTriangles = 0;
  for (auto poly : polygons) {
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLdouble)*vboOffset*3,
                    sizeof(GLdouble)*poly.vertices.size()*3, &poly.vertices[0]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*iboOffset*3,
                    sizeof(GLuint)*poly.indices.size()*3, &poly.indices[0]);
    vboOffset += poly.vertices.size();
    iboOffset += poly.indices.size();
    _numTriangles += poly.indices.size()*3;
    _polygons.push_back(poly);
  }
  if (polygons.size() > 1) {
    std::cout << name << std::endl;
  }
  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble)*3,
      (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}
