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

void Drawable::addPolygon ( std::vector<Vertex> const & vertData,
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

void Polygon::addData ( std::vector<Vertex> const & vertData,
                        std::vector<IVec3> const & triangleData )
{
  _vertices = vertData;
  _triangles = triangleData;
  for (auto & index : _triangles) {
    glm::dvec3 vec1 = _vertices[index[1]].position - _vertices[index[0]].position;
    glm::dvec3 vec2 = _vertices[index[2]].position - _vertices[index[0]].position;
    glm::dvec3 normal = glm::normalize(glm::cross(vec1, vec2));
    _vertices[index[0]].normal += normal;
    _vertices[index[1]].normal += normal;
    _vertices[index[2]].normal += normal;
  }
  for (auto &  vertex : _vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }
  _vertices.resize(_vertices.size()*2);
  for (unsigned int i = _vertices.size()/2; i < _vertices.size(); i++) {
    _vertices[i] = _vertices[i-_vertices.size()/2];
    _vertices[i].position[1] += 0.5f;
  }
  _triangles.resize(_triangles.size()*2);
  for (unsigned int i = _triangles.size()/2; i < _triangles.size(); i++) {
    _triangles[i] = _triangles[i-_triangles.size()/2];
    _triangles[i][0] += _vertices.size()/2;
    _triangles[i][1] += _vertices.size()/2;
    _triangles[i][2] += _vertices.size()/2;
  }
  for (unsigned int i = 0; i < _vertices.size()/2; i++) {
    unsigned int j = i+1, p, q;
    if (i == (_vertices.size()/2)-1)
      j = 0;
    p = i+_vertices.size()/2;
    q = j+_vertices.size()/2;
    _triangles.push_back(IVec3(p, i, j));
    _triangles.push_back(IVec3(p, j, q));
  }

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ibo);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLdouble)*_vertices.size()*6,
      &_vertices[0], GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*_triangles.size()*3,
      &_triangles[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble)*6,
      (void *)0);
  glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, sizeof(GLdouble)*6,
      (void *)(sizeof(GLdouble)*3));

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Polygon::draw ()
{
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
  glDrawElements(GL_TRIANGLES, GLint(_triangles.size())*3, GL_UNSIGNED_INT,
      (void *)0);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}
