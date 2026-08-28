#version 410 core

in VertexShaderOutput
{
  vec3 view_position;
  vec3 view_normal;
}
vso;

layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;

void main()
{
  frag_position = vec4(vso.view_position, 1.0);
  frag_normal = vec4(normalize(vso.view_normal), 0.0);
}
