// resources/shaders/particle.vert
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in float aLife;

uniform mat4 projection;
uniform mat4 view;

out float life;

void main() {
    life = aLife;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize * (1.0 / gl_Position.w) * 1000.0;  // Scale with distance
}
