#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 Normal;
out vec3 FragPos;
out vec2 UV;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
    gl_Position = mvp * vec4(pos, 1.0);

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * normal);
    UV = uv;
}