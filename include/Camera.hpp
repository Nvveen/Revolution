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
