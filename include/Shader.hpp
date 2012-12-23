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
#ifndef  SHADER_HPP
#define  SHADER_HPP

#include <sstream>
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

#endif
