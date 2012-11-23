#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <GL/gl.h>
#include "Shader.hpp"
#include "Camera.hpp"

class World {
  public:
    World () {}
    World (const unsigned int & width, const unsigned int & height);
    virtual ~World();

    void draw();
    Camera & getCamera();
  protected:
  private:
    void initVertices();
    void initShaderProgram();

    Shader *_shader;
    Camera *_cam;
    GLuint _vbo;
    GLuint _vao;


    std::vector<GLfloat> data;
    glm::mat4 model;
    glm::mat4 mvp;
};

inline Camera & World::getCamera () {
  return *(this->_cam);
}

#endif
