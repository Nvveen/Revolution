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
#include "DataManager.hpp"

struct DirectionalLight;

class World {
  public:
    World () {}
    World (const unsigned int & width, const unsigned int & height);
    virtual ~World();

    void draw ();
    Camera & getCamera();

    void showMore ();
    void showLess ();
    void changeDiffuseIntensity (float const & val);
    void changeAmbientIntensity (float const & val );
    DirectionalLight getDefaultLight ();

    void selectCountry (int const & x, int const & y);
    void raiseSelected ( float const & amount );
    Drawable * getCountry ( std::string const & name );
    void activateDataset ( unsigned int const & index );
    void deactivateDataset ( unsigned int const & index );
  protected:
  private:
    Shader *_shader;
    Camera *_cam;
    DirectionalLight *_dLight0;
    int _show = 0;
    int _maxDrawable = 0;
    std::vector<Drawable *> _drawables;
    Drawable *_selected;

    std::vector<DataManager *> _datasets;
    
    void init ();
    void readDatasets ();
};

struct DirectionalLight {
  glm::vec3 Color, Direction;
  float AmbientIntensity, DiffuseIntensity;
};

inline Camera & World::getCamera ()
{
  return *(this->_cam);
}

inline void World::showMore ()
{
  _maxDrawable++;
  std::cout << "Showing " << _maxDrawable << std::endl;
}

inline void World::showLess ()
{
  _maxDrawable--;
  std::cout << "Showing " << _maxDrawable << std::endl;
}

inline void World::changeDiffuseIntensity ( float const & val )
{
  _dLight0->DiffuseIntensity = val;
}

inline void World::changeAmbientIntensity ( float const & val )
{
  _dLight0->AmbientIntensity = val;
}

inline DirectionalLight World::getDefaultLight ()
{
  DirectionalLight d;
  d.Color = glm::vec3(1, 1, 1);
  d.Direction = glm::normalize(glm::vec3(0, -50, 0));
  d.AmbientIntensity = 0.5f;
  d.DiffuseIntensity = 0.5f;
  return d;
}

inline void World::activateDataset ( unsigned int const & index )
{
  _datasets[index]->activate(2012);
}

inline void World::deactivateDataset ( unsigned int const & index )
{
  _datasets[index]->deactivate();
}

#endif
