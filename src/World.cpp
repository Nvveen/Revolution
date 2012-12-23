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
#include <boost/filesystem.hpp>
#include "World.hpp"

World::World(const unsigned int & width, const unsigned int & height)
{
  _cam = new Camera(width, height);
  _dLight0 = new DirectionalLight;
  *_dLight0 = getDefaultLight();
  init();
}

World::~World()
{
  delete _cam;
  delete _shader;
  delete _dLight0;
  for (auto p : _drawables)
    delete p;
  for (auto set : _datasets)
    delete set;
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
  _shader->setUniformLocation("vWorld");
  _shader->setUniformLocation("objectColor");
  _shader->setUniformLocation("height");
  _shader->setUniformLocation("dLight0.Color");
  _shader->setUniformLocation("dLight0.AmbientIntensity");
  _shader->setUniformLocation("dLight0.DiffuseIntensity");
  _shader->setUniformLocation("dLight0.Direction");

  std::cout << "Loading countries..." << std::endl;

  // Read country list
  std::ifstream countryList("share/countries/countrylist");
  std::vector<std::string> names;
  while (!countryList.eof()) {
    std::string name;
    getline(countryList, name);
    if (name.size() > 0) {
      names.push_back(name);
    }
  }
  countryList.close();
  for (auto name : names) {
    std::string pathName = "share/countries/" + name + "/";
    namespace bfs = boost::filesystem;
    bfs::path p(pathName);
    glm::vec4 randomColor(0.0f);
    while (randomColor[0] < 0.1f && randomColor[1] < 0.1f &&
        randomColor[2] < 0.1f) {
      randomColor = glm::vec4(glm::vecRand3(0.0f, 1.0f), 1.0f);
    }
    Drawable *country = new Drawable(_shader, randomColor);
    country->name = name;
    try {
      if (bfs::exists(p)) {
        if(bfs::is_directory(p)) {
          bfs::directory_iterator it(p), end_it;
          // Iterate over each file in each directory, a polygon.
          for (; it != end_it; ++it) {
            std::ifstream regionFile(it->path().string());
            std::string name;
            regionFile.ignore(2);
            regionFile >> name;
            regionFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            int vertSize = 0, triangleSize = 0;
            regionFile >> vertSize >> triangleSize;
            regionFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            regionFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::vector<Vertex> vert;
            std::vector<IVec3> ind;
            for (int i = 0; i < vertSize; i++) {
              std::string str;
              getline(regionFile, str);
              std::stringstream line(str);
              if (str.size() < 5 || str[0] == '#')
                std::cout << str << std::endl;
              Vertex v;
              getline(line, str, ',');
              std::stringstream(str) >> v.position[0];
              v.position[0] = -v.position[0];
              getline(line, str);
              std::stringstream(str) >> v.position[2];
              v.position[1] = 0;
              vert.push_back(v);
            }
            regionFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            for (int i = 0; i < triangleSize; i++) {
              std::string str;
              getline(regionFile, str);
              std::stringstream line(str);
              IVec3 vec;
              getline(line, str, ',');
              std::stringstream(str) >> vec[0];
              getline(line, str, ',');
              std::stringstream(str) >> vec[1];
              getline(line, str);
              std::stringstream(str) >> vec[2];
              ind.push_back(vec);
            }
            country->addPolygon(vert, ind);
          }
        }
      } else {
        std::cerr << pathName << " does not exist." << std::endl;
      }
      _drawables.push_back(country);
    }
    catch (bfs::filesystem_error const & e) {
      std::cerr << e.what() << std::endl;
    }
  }

  std::cout << "Done loading countries..." << std::endl;
  std::cout << "Loaded " << names.size() << " countries." << std::endl;
  _maxDrawable = _drawables.size();
  std::cout << "Loading datasets... " << std::endl;
  readDatasets();
}

void World::draw()
{
  _shader->bind();
  _shader->setUniform("dLight0.Color", _dLight0->Color);
  _shader->setUniform("dLight0.AmbientIntensity", _dLight0->AmbientIntensity);
  _shader->setUniform("dLight0.Direction", _dLight0->Direction);
  _shader->setUniform("dLight0.DiffuseIntensity", _dLight0->DiffuseIntensity);
  _shader->setUniform("vWorld", glm::mat4(1.0));
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  int i = 0;
  for (auto p : _drawables) {
    glStencilFunc(GL_ALWAYS, i+1, -1);
    if (i < _maxDrawable)
      p->draw(_cam);
    i++;
  }
}

void World::selectCountry (int const & x, int const & y)
{
  unsigned int index;
  glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
  if (index != 0) {
    _selected = _drawables[index-1];
    std::cout << "Selected " << _selected->name << std::endl;
  }
}

void World::raiseSelected(float const & amount)
{
  if (_selected != NULL) {
    std::cout << "Raising " << _selected->name << " by " << amount << std::endl;
    _selected->setHeightDistortion(amount);
  }
}

void World::readDatasets ()
{
  namespace bfs = boost::filesystem;
  bfs::path p("share/datasets/");
  try {
    if (bfs::exists(p)) {
      bfs::directory_iterator it(p), end_it;
      for (; it != end_it; ++it) {
        std::cout << "File: " << *it << std::endl;
      }
    } else {
      std::cerr << p << " does not exist." << std::endl;
    }
  }
  catch (bfs::filesystem_error const & e) {
    std::cerr << e.what() << std::endl;
  }
}
