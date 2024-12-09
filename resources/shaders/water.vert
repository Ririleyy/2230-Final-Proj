#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec2 TexCoord;
out vec2 DispCoord1;  // First displacement layer
out vec2 DispCoord2;  // Second displacement layer for pooling effect
out vec2 WaveCoord;   // Additional wave coordinate for more detail

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;

    // Base water movement with increased speed
    DispCoord1 = aTexCoord + vec2(time * 0.1, time * 0.08);

    // Slower, different direction for pooling effect
    DispCoord2 = aTexCoord + vec2(-time * 0.05, time * 0.07);

    // Fast moving waves for additional detail
    WaveCoord = aTexCoord * 2.0 + vec2(time * 0.15);
}
