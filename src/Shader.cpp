#include <fstream>
#include <sstream>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"

Shader::Shader() : _bound(false) {
    // Create the shader program
    _shaderProgram = 0;
}

Shader::~Shader() {
  for ( auto it : _shaderObjects )
      glDeleteShader(it.second);
  glDeleteProgram(_shaderProgram);
}

void Shader::add( std::string fileName, GLenum type ) {
  try {
    if ( _shaderProgram == 0 ) _shaderProgram = glCreateProgram();

    std::string code = addCode(fileName);
    GLuint object = compileShader(code, type);
    _shaderObjects.insert(std::pair<GLenum, GLuint>(type, object));
    // Bind the shader to the program.
    glAttachShader(_shaderProgram, object);
  }
  catch (ShaderException e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}

std::string Shader::addCode( std::string fileName ) {
  try {
    std::stringstream shaderCode(std::ios::out);
    std::ifstream file;
    file.open(fileName.c_str(), std::ios::in);

    shaderCode << file.rdbuf();
    return shaderCode.str();
  }
  catch ( std::ifstream::failure e ) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}


void Shader::link() {
  try {
    GLint success;
    // Link program.
    glLinkProgram(_shaderProgram);
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    // Error check for linking
    if ( success == GL_FALSE ) {
        GLchar errorLog[1024];
        glGetProgramInfoLog(_shaderProgram, sizeof(errorLog), NULL, errorLog);
        throw(ShaderException("Error linking shader program: "+
              std::string(errorLog), _shaderProgram));
    }

    glValidateProgram(_shaderProgram);
    glGetProgramiv(_shaderProgram, GL_VALIDATE_STATUS, &success);
    // Error check for validation
    if ( success == GL_FALSE ) {
        GLchar errorLog[1024];
        glGetProgramInfoLog(_shaderProgram, sizeof(errorLog), NULL, errorLog);
        throw(ShaderException("Invalid shader program: "+std::string(errorLog),
              _shaderProgram));
    }
    _bound = true;
    unbind();
  }
  catch (ShaderException e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}


GLuint Shader::compileShader( std::string code, GLenum type ) {
  try {
    GLuint shader = glCreateShader(type);
    // Error checking
    if ( shader == 0 )
        throw("Error creating shader type "+type);

    // Convert string to const char.
    const GLchar *sStringPtr[1];
    sStringPtr[0] = code.c_str();
    // Pass code to shader
    glShaderSource(shader, 1, (const GLchar **)sStringPtr, NULL);

    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    // Error checking for compilation.
    if ( success == GL_FALSE ) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::ostringstream ss;
        ss << "Error compiling shader " << type << ": " << infoLog;
        ss << " in shader " << _shaderProgram;
        throw(ShaderException(ss.str(), _shaderProgram));
    }
    return shader;
  }
  catch ( ShaderException e ) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}

bool Shader::setUniform ( std::string name, GLfloat val ) {
    if ( _shaderProgram && _bound )
        glUniform1i(_uniformLocs[name], val);
    return true;
}

bool Shader::setUniform ( std::string name, glm::vec3 vec ) {
    if ( _shaderProgram && _bound )
        glUniform3f(_uniformLocs[name], vec.x, vec.y, vec.z);
    return true;
}


bool Shader::setUniform ( std::string name, glm::vec4 vec ) {
    if ( _shaderProgram && _bound )
        glUniform4f(_uniformLocs[name], vec.x, vec.y, vec.z, vec.w);
    return true;
}

bool Shader::setUniform ( std::string name, glm::mat4 matrix ) {
    if ( _shaderProgram && _bound )
        glUniformMatrix4fv(_uniformLocs[name], 1, GL_FALSE, 
                           glm::value_ptr(matrix));
    return true;
}

