#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec2 DispCoord;

uniform sampler2D dispTexture;  // Displacement map
uniform float dispStrength;     // Displacement strength


void main() {
    // Enhance displacement effect
    vec2 displacement = (texture(dispTexture, DispCoord).rg * 2.0 - 1.0) * dispStrength * 2.0;

    vec2 distortedCoord = TexCoord + displacement;

    // Use two-tone water color for better visual effect
    vec3 deepWater = vec3(0.0, 0.2, 0.8);  // Deep water color
    vec3 shallowWater = vec3(0.1, 0.4, 0.9);  // Shallow water color

    // Create color variation based on displacement
    float waveIntensity = length(displacement) * 8.0;
    vec3 finalColor = mix(deepWater, shallowWater, waveIntensity);

    // Enhanced edge fade calculation
    vec2 center = vec2(0.5, 0.5);
    vec2 pos = distortedCoord - center;
    float ellipticalDist = length(vec2(pos.x * 1.5, pos.y * 2.0));
    float alpha = smoothstep(0.8, 0.3, ellipticalDist) * 0.7;  // Increased alpha

    FragColor = vec4(finalColor, alpha);
}

