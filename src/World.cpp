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
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/random.hpp>
#include "World.hpp"

World::World(const unsigned int & width, const unsigned int & height)
{
  _cam = new Camera(width, height);
  init();
}

World::~World()
{
  delete _cam;
  delete _shader;
  for (auto p : _drawables)
    delete p;
}

void World::init ()
{
  std::cout << "Initializing shaders..." << std::endl;
  _shader = new Shader;
  const std::string p = "share/shaders/world.";
  _shader->add(p+"vs", GL_VERTEX_SHADER);
  _shader->add(p+"fs", GL_FRAGMENT_SHADER);
  _shader->link();
  _shader->setUniformLocation("vMVP");
  _shader->setUniformLocation("objectColor");

  std::cout << "Loading countries..." << std::endl;

  std::ifstream file;
  file.open("share/out.dat", std::ios::binary);
  unsigned short numCountries = 0;
  file.read(reinterpret_cast<char *>(&numCountries), sizeof(unsigned short));
  for (unsigned short i = 0; i < numCountries; i++) {
    glm::vec4 randomColor = glm::vec4(glm::vecRand3(0.0f, 1.0f), 1.0f);
    Drawable *d = new Drawable(_shader, randomColor);
    unsigned short nameLength, numPolygons;
    std::vector<Vec3> verts;
    std::vector<IVec3> ind;
    file.read(reinterpret_cast<char *>(&nameLength), sizeof(unsigned short));
    char *name = new char[nameLength+1];
    file.read(reinterpret_cast<char *>(&name[0]), nameLength);
    name[nameLength] = '\0';
    file.read(reinterpret_cast<char *>(&numPolygons), sizeof(unsigned short));
    for (unsigned short j = 0; j < numPolygons; j++) {
      unsigned int numVerts, numEdges;
      file.read(reinterpret_cast<char *>(&numVerts), sizeof(unsigned int));
      for (unsigned int k = 0; k < numVerts; k++) {
        double x, y;
        file.read(reinterpret_cast<char *>(&x), sizeof(double));
        file.read(reinterpret_cast<char *>(&y), sizeof(double));
        verts.push_back(Vec3(x, -10, y));
      }
      file.read(reinterpret_cast<char *>(&numEdges), sizeof(unsigned int));
      for (unsigned int k = 0; k < numEdges; k++) {
        unsigned int x, y, z;
        file.read(reinterpret_cast<char *>(&x), sizeof(unsigned int));
        file.read(reinterpret_cast<char *>(&y), sizeof(unsigned int));
        file.read(reinterpret_cast<char *>(&z), sizeof(unsigned int));
        ind.push_back(IVec3(x, y, z));
      }
    }
    d->addData(verts, ind);
    d->name = std::string(name);
    std::cout << d->name << std::endl;
    _drawables.push_back(d);
  }
  std::cout << "Done loading countries..." << std::endl;
}

void World::draw()
{
  for (auto p : _drawables) {
    p->draw(_cam);
  }
}
