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
    Camera (const unsigned int & width, const unsigned int & height);
    virtual ~Camera(){}

    glm::mat4 transform( const glm::mat4 & model );
    void moveForward();
    void move( const float & deltaX, const float & deltaY );
    void look( const float & refX, const float & refY, const float & i,
        const float & j );
  protected:
  private:
    glm::vec3 _dir, _pos;
    float _horizontalAngle, _verticalAngle;
    float _initialFoV;
    float _ratio;

    glm::mat4 getProjection();
    glm::mat4 getView();
    glm::vec3 getDirection();
    glm::vec3 getRight();
};

inline glm::mat4 Camera::transform( const glm::mat4 & model ) {
  return model * getProjection() * getView();
}

inline glm::vec3 Camera::getDirection() {
  return glm::vec3(
      cos(_verticalAngle) * sin(_horizontalAngle),
      sin(_verticalAngle),
      cos(_verticalAngle) * cos(_horizontalAngle));
}

inline glm::vec3 Camera::getRight() {
  return glm::vec3(
      sin(_horizontalAngle - M_PI/2.0f),
      0,
      cos(_horizontalAngle - M_PI/2.0f));
}

#endif
