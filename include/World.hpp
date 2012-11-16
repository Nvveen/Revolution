#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <GL/gl.h>
#include "Shader.hpp"

class World {
  public:
    World ();
    virtual ~World() {}

    void draw();
  protected:
  private:
    void initVertices();
    void initShaderProgram();

    Shader *_shader;
    GLuint _vbo;
};

#endif
