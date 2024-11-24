#version 330 core

in vec3 vert_color;

out vec4 FragColor;

void main() {
  FragColor = vec4(1.0, 0.5, 0.2, 1.0);
  // FragColor = vec4(vert_color, 1.0);
}
