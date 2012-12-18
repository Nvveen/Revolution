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
#include <algorithm>
#include "Drawable.hpp"

Drawable::Drawable ()
{
}

Drawable::Drawable ( Shader * const shader, glm::vec4 const & primary ) : 
  _shader(shader), _primaryColor(primary), _model(glm::mat4(1.0)),
  _heightDistortion(0)
{
}

Drawable::~Drawable ()
{
}

void Drawable::draw ( Camera * const cam )
{
  _shader->bind();
  glm::mat4 mvp = cam->transform(_model);
  _shader->setUniform("vMVP", mvp);
  _shader->setUniform("objectColor", _primaryColor);
  _shader->setUniform("height", _heightDistortion);

  for (auto polygon : _polygons) {
    polygon.draw();
  }

  _shader->unbind();
}

void Drawable::addPolygon ( std::vector<Vec3> const & vertData,
                             std::vector<IVec3> const & indData )
{
  _polygons.push_back(Polygon());
  _polygons.back().addData(vertData, indData);

  if (name == "Spain") {
    _heightDistortion = 10.0f;
    std::cout << "Spain can go fuck itself." << std::endl;
  }
}

Polygon::Polygon ()
{
}

void Polygon::addData ( std::vector<Vec3> const & vertData,
                        std::vector<IVec3> const & indData )
{
  vertices = vertData;
  indices = indData;
  vertices.resize(vertices.size()*2);
  for (unsigned int i = vertices.size()/2; i < vertices.size(); i++) {
    vertices[i] = vertices[i-vertices.size()/2];
    vertices[i][1] += 0.5f;
  }
  indices.resize(indices.size()*2);
  for (unsigned int i = indices.size()/2; i < indices.size(); i++) {
    indices[i] = indices[i-indices.size()/2];
    indices[i][0] += vertices.size()/2;
    indices[i][1] += vertices.size()/2;
    indices[i][2] += vertices.size()/2;
  }
  for (unsigned int i = 0; i < vertices.size()/2; i++) {
    unsigned int j = i+1, p, q;
    if (i == (vertices.size()/2)-1)
      j = 0;
    p = i+vertices.size()/2;
    q = j+vertices.size()/2;
    indices.push_back(IVec3(p, i, j));
    indices.push_back(IVec3(p, j, q));
  }

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ibo);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLdouble)*vertices.size()*3,
      &vertices[0], GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size()*3,
      &indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble)*3,
      (void *)0);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Polygon::draw ()
{
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glDrawElements(GL_TRIANGLES, GLint(indices.size())*3, GL_UNSIGNED_INT,
      (void *)0);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}
