#version 330 core

uniform vec3 mesh_color;

out vec4 FragColor;

void main()
{
  FragColor = vec4(mesh_color, 1.0);
}
