#ifndef  SHADER_HPP
#define  SHADER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <stdexcept>
#include <GL/glew.h>

class Shader
{
    public:
        Shader ();
        ~Shader ();

        void add ( std::string fileName, GLenum type );
        void link ();
        void bind ();
        void unbind ();
        GLuint getShaderProgram();
        bool setUniform ( std::string name, GLfloat val );
        bool setUniform ( std::string name, glm::vec3 vec );
        bool setUniform ( std::string name, glm::vec4 vec );
        bool setUniform ( std::string name, glm::mat4 matrix );
        void setUniformLocation ( std::string name );
    private:
        std::map<GLenum, GLuint> _shaderObjects;
        GLuint _shaderProgram;

        std::map<std::string, GLint> _uniformLocs;

        std::string addCode ( std::string fileName );
        GLuint      compileShader ( std::string code, GLenum type );

        bool _bound;
};

class ShaderException : public std::runtime_error {
  public:
    ShaderException( const std::string & what, GLuint prog ) :
      std::runtime_error(what), _program(prog) {}
    const char *what() const throw();
  private:
    GLuint _program;
};

inline const char *ShaderException::what () const throw() {
  std::ostringstream ss;
  ss << "Shader exception in " << _program << ": ";
  ss << std::runtime_error::what();
  return ss.str().c_str();
}

inline GLuint Shader::getShaderProgram() {
    return _shaderProgram;
}

inline void Shader::bind()
{
    if ( !_bound )
        glUseProgram(_shaderProgram);
    _bound = true;
}

inline void Shader::unbind() {
    if ( _bound )
        glUseProgram(0);
    _bound = false;
}

inline void Shader::setUniformLocation( std::string name ) {
    bind();
    try {
        GLint loc = glGetUniformLocation(_shaderProgram, name.c_str());
        _uniformLocs[name] = loc;
        if ( loc == -1 ) throw true;
        GLenum error = glGetError();
        if ( error != GL_NO_ERROR ) throw error;
    }
    catch ( bool e ) {
        if ( e ) {
            std::cerr << "Error finding uniform " << name;
            std::cerr << " in shaderprogram " << _shaderProgram << "\n";
        }
    }
    catch ( GLenum error ) {
        if ( error == GL_INVALID_VALUE ) {
            std::cerr << "Invalid value " << _shaderProgram << " used.\n";
        }
        else if ( error == GL_INVALID_OPERATION ) {
            std::cerr << "Invalid operation while setting uniform ";
            std::cerr << name << " in shaderProgram " << _shaderProgram << "\n";
        }
    }
    unbind();
}

#endif
