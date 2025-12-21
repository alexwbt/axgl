#version 420 core

layout (location = 0) in vec2 position;

//uniform vec2 position;
//uniform vec2 size;
//uniform vec4 margin;
//uniform vec4 padding;
//uniform vec4 color;
//uniform vec4 border_color;
//uniform float border_width;
//uniform float border_radius;

void main()
{
  gl_Position = vec4(-position.x, position.y, 0.0, 1.0);
}
