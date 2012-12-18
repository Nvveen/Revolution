/*
This file is part of Revolution.

Revolution is free software: you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

Revolution is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
more details.

You should have received a copy of the GNU General Public License along with 
Revolution. If not, see <http://www.gnu.org/licenses/>.
*/
#version 330

layout(location = 0) in vec3 position;

uniform mat4 vMVP;
uniform float height;

void main() {
  vec4 pos = vec4(position, 1.0f);
  if (fract(pos.y) != 0.0) {
    pos.y += height;
  }
  gl_Position = vMVP * pos;
}

