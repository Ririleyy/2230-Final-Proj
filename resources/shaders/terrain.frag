#version 330 core

in vec2 fragUV; // UV coordinates from vertex shader
in float height; // Height from vertex shader
uniform sampler2D texture1; // Texture sampler

out vec4 fragColor;

void main() {
    vec3 texColor = texture(texture1, fragUV).rgb; // Sample the texture using UV coordinates
    float finalHeight = clamp(0.8 - (height / 100), 0.0, 1.0); // Clamp the height between 0 and 1
    fragColor = vec4(finalHeight, finalHeight, finalHeight, 1.0); // Output the texture color

}
