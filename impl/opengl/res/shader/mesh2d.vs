#version 330 core

layout (location = 0) in vec2 pos;

uniform float scale;
uniform vec2 offset;
uniform vec2 viewport;

void main()
{
  vec2 screen_pos = (pos + offset) * scale / viewport;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}
