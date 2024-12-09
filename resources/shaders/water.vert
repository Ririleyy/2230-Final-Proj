#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;  // Add time uniform

out vec2 TexCoord;
out vec2 DispCoord;  // For displacement mapping


void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    // Increase scrolling speed and add some variation
    DispCoord = aTexCoord + vec2(time * 0.05, time * 0.05);  // Different speeds for x and y
}
