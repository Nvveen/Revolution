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
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"

Camera::Camera (const unsigned int & width, const unsigned int & height) {
  _initialFoV = 45.0f;
  _ratio = float(width)/float(height);

  _horizontalAngle = float(M_PI);
  _verticalAngle = 45.0f;
  _pos = glm::vec3(0, 5, 5);
}

void Camera::move( const float & deltaX, const float & deltaY ) {
  _pos += glm::vec3(0.2*deltaX, 0, 0.2*deltaY);
  std::cout << _pos[0] << " " << _pos[1] << " " << _pos[2] << std::endl;
}

void Camera::look( const float & refX, const float & refY, const float & i,
    const float & j ) {
  float mouseSpeed = 0.005f;
  _horizontalAngle += mouseSpeed * (refX-i);
  _verticalAngle += mouseSpeed * (refY-j);
}

void Camera::zoom ( float const & val )
{
  _initialFoV -= val;
  getProjection();
}

glm::mat4 Camera::getProjection() {
  float fov = _initialFoV;
  return glm::perspective(fov, _ratio, 0.1f, 100.0f);
}

glm::mat4 Camera::getView() {
  glm::vec3 direction = getDirection();
  glm::vec3 right = getRight();
  glm::vec3 up = glm::cross(right, direction);

  return glm::lookAt(_pos, _pos+direction, up);
}
