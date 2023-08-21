#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 pv;

void main()
{
    gl_Position = pv * vec4(pos, 1.0);
}