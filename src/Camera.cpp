#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"

Camera::Camera (const unsigned int & width, const unsigned int & height) {
  _initialFoV = 45.0f;
  _ratio = float(width)/float(height);

  _horizontalAngle = M_PI;
  _verticalAngle = -45.0f;
  _pos = glm::vec3(0, 5, 5);
}

void Camera::move( const float & deltaX, const float & deltaY ) {
  _pos += glm::vec3(0.2*deltaX, 0, 0.2*deltaY);
}

void Camera::look( const float & refX, const float & refY, const float & i,
    const float & j ) {
  float mouseSpeed = 0.005f;
  _horizontalAngle += mouseSpeed * (refX-i);
  _verticalAngle += mouseSpeed * (refY-j);
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
