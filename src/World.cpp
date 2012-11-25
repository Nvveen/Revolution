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

  _drawables.push_back(new Drawable(_shader, glm::vec4(1, 0, 0, 1)));
}

void World::draw()
{
  for (auto p : _drawables) {
    p->draw(_cam);
  }
}
