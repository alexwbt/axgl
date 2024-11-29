#version 330 core

uniform vec3 mesh_color = vec3(1.0, 1.0, 1.0);

out vec4 FragColor;

void main()
{
  FragColor = vec4(mesh_color, 1.0);
}
