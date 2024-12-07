#version 330 core

in vec2 fragUV; // UV coordinates from vertex shader
uniform sampler2D texture1; // Texture sampler

out vec4 fragColor;

void main() {
    vec3 texColor = texture(texture1, fragUV).rgb; // Sample the texture using UV coordinates
    fragColor = vec4(texColor, 1.0); // Output the texture color

}
