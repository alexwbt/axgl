#version 460 core

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

uniform vec3 color;

out vec4 fColor;

void main() {
    gl_Position = gl_in[1].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    fColor = vec4(color, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    fColor = vec4(color, 0.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fColor = vec4(color, 1.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    fColor = vec4(color, 1.0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    fColor = vec4(color, 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    fColor = vec4(color, 0.0);
    EmitVertex();

    EndPrimitive();
}
