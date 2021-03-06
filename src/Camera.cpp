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

#include "Camera.hpp"
#include <boost/math/constants/constants.hpp>

#define pi boost::math::constants::pi<double>();

Camera::Camera ( unsigned int const & width, unsigned int const & height )
{
  _projection = getProjection(width, height);

  _pos = glm::vec3(0, 50.0f, -50.0f);
  _up = glm::vec3(0.0, 1.0f, 0.0f);

  _horizontalAngle = 0.0f;
  _verticalAngle = -45.0f/360.0f * pi;
  _target = getDirection();
}

void Camera::move ( float const & dX, float const & dY, float const & dZ )
{
  glm::vec3 forward = glm::normalize(_target - _pos);
  forward[1] = 0.0f;
  glm::vec3 translation = dX * glm::normalize(glm::cross(_up, forward)) +
    dY * _up + dZ * forward;
  _pos += translation;
  _target += translation;
}

void Camera::look ( float const & refX, float const & refY, float const & i,
                    float const & j )
{
  float mouseSpeed = 0.001f;
  _horizontalAngle += mouseSpeed * (refX-i);
  _verticalAngle += mouseSpeed * (refY-j);
  glm::vec3 forward = getDirection();
  _target = _pos + forward;
}

void Camera::resize( unsigned int const & width, unsigned int const & height ) {
  _projection = getProjection(width, height);
}
