#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "World.hpp"

World::World() {
  initVertices();
}

void World::initShaderProgram() {
  _shader = new Shader;
  const std::string p = "share/shaders/world.";
  _shader->add(p+"vs", GL_VERTEX_SHADER);
  _shader->add(p+"fs", GL_FRAGMENT_SHADER);
  _shader->link();
}

void World::draw() {
  _shader->bind();
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4,
  //    (void *)(sizeof(GLfloat)*4));
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  _shader->unbind();
}

void World::initVertices() {
  GLfloat pos[] = {
    0.75f, 0.75f, 0.5f, 1.0f,
    //1.0f, 0.0f, 0.0f, 1.0f,
    0.75f, -0.75f, 0.5f, 1.0f,
    //0.0f, 1.0f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.5f, 1.0f,
    //0.0f, 0.0f, 1.0f, 1.0f,
  };
  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
