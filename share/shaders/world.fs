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

in vec3 normal0;
out vec4 color;

uniform vec4 objectColor;

struct DirectionalLight {
  vec3 Color;
  float AmbientIntensity;
  float DiffuseIntensity;
  vec3 Direction;
};

uniform DirectionalLight dLight0;

void main() {
  vec4 AmbientColor = vec4(dLight0.Color, 1.0f) * dLight0.AmbientIntensity;
  float DiffuseFactor = dot(normalize(normal0), -dLight0.Direction);
  vec4 DiffuseColor;
  if (DiffuseFactor > 0) {
    DiffuseColor = vec4(dLight0.Color, 1.0f) * dLight0.DiffuseIntensity *
      DiffuseFactor;
  }
  else {
    DiffuseColor = vec4(0, 0, 0, 0);
  }
  color = objectColor * (AmbientColor + DiffuseColor);
}
