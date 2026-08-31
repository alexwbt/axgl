#version 410 core

uniform vec3 light_position;
uniform float far_plane;

in vec3 world_position;

void main()
{
  float light_distance = length(world_position - light_position);
  light_distance /= far_plane;
  gl_FragDepth = light_distance;
}
