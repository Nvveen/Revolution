#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "World.hpp"

World::World(const unsigned int & width, const unsigned int & height) {
  initVertices();
  initShaderProgram();

  _cam = new Camera(width, height);

  model = glm::mat4(1.0f);
}

World::~World() {
  delete _cam;
  delete _shader;
}

void World::initShaderProgram() {
  std::cout << "Initializing shaders..." << std::endl;
  _shader = new Shader;
  const std::string p = "share/shaders/world.";
  _shader->add(p+"vs", GL_VERTEX_SHADER);
  _shader->add(p+"fs", GL_FRAGMENT_SHADER);
  _shader->link();
  _shader->setUniformLocation("vMVP");
}

void World::draw() {
  _shader->bind();
  mvp = _cam->transform(model);
  _shader->setUniform("vMVP", mvp);

  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glDrawArrays(GL_TRIANGLES, 0, data.size()/7);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  _shader->unbind();
}

void World::initVertices() {
  std::cout << "Initializing world model..." << std::endl;

  data = std::vector<GLfloat>{
    // Front
    -1.0f, -1.0f, -1.0f,
     1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,
     0.0f, 1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, -1.0f,
     0.0f, 1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,
     0.0f, 1.0f, 0.0f, 1.0f,
    // Right
     1.0f, -1.0f, -1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    // Back
     1.0f, -1.0f,  1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    // Left
    -1.0f, -1.0f,  1.0f,
    1.0f, 1.0f, 0.5f, 1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f, 1.0f, 0.5f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 0.5f, 1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f, 0.5f, 1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,
    1.0f, 0.5f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, 0.5f, 1.0f, 1.0f,
    // Top
    -1.0f,  1.0f, -1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
     1.0f,  1.0f, -1.0f,
    1.0f, 0.5f, 0.5f, 1.0f,
    -1.0f,  1.0f,  1.0f,
    0.5f, 1.0f, 1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,
    0.5f, 1.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,
    0.5f, 1.0f, 1.0f, 1.0f,
    // Bottom
    -1.0f, -1.0f,  1.0f,
    0.5f, 1.0f, 0.5f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    0.5f, 1.0f, 0.5f, 1.0f,
     1.0f, -1.0f, -1.0f,
    0.5f, 1.0f, 0.5f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
     1.0f, -1.0f,  1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
  };

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*data.size(), &data[0],
      GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7,
      (void *)0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*7,
      (void *)(sizeof(GLfloat)*3));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}
