#version 330 core

in vec3 outColor; // UV coordinates from vertex shader
in float height; // Height from vertex shader
uniform sampler2D texture1; // Texture sampler

out vec4 fragColor;

void main() {
    fragColor = vec4(outColor, 1.0);
}
