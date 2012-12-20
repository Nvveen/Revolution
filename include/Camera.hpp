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
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
  public:
    Camera (){}
    Camera ( unsigned int const & width, unsigned int const & height );
    virtual ~Camera (){}

    glm::mat4 transform ( glm::mat4 const & model );
    void move ( float const & dX, float const & dY, float const & dZ );
    void look ( float const & rX, float const & rY, float const & i,
                float const & j );

  protected:
  private:
    glm::vec3 _pos, _target, _up;
    glm::mat4 _projection;
    float _horizontalAngle, _verticalAngle;

    glm::mat4 getProjection();
    glm::mat4 getView();
    glm::vec3 getDirection();
};

inline glm::mat4 Camera::transform ( glm::mat4 const & model )
{
  return model * _projection * getView();
}

inline glm::vec3 Camera::getDirection ()
{
  return glm::vec3(
      cos(_verticalAngle) * sin(_horizontalAngle),
      sin(_verticalAngle),
      cos(_verticalAngle) * cos(_horizontalAngle));
}

#endif
