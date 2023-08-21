#version 460 core

layout (location = 0) in vec3 pos;

out vec3 TexCoords;

uniform mat4 pv;

void main()
{
    TexCoords = pos;
    gl_Position = pv * vec4(pos, 1.0);
}
