// #version 330 core
// out vec4 FragColor;

// in vec2 TexCoord;
// in vec2 DispCoord1;
// in vec2 DispCoord2;
// in vec2 WaveCoord;

// uniform sampler2D dispTexture;
// uniform float dispStrength;
// uniform float time;

// // Helper function to process water layers
// float processWaterLayer(vec2 coord) {
//     // Get raw value from displacement map
//     vec2 dispValue = texture(dispTexture, coord).rg;
//     // Convert to -1 to 1 range and enhance effect
//     return (length(dispValue) * 2.0 - 1.0);
// }

// void main() {
//     // Process three separate layers with adjusted strengths
//     float baseLayer = processWaterLayer(DispCoord1) * dispStrength;
//     float poolLayer = processWaterLayer(DispCoord2) * (dispStrength * 0.7);
//     float waveLayer = processWaterLayer(WaveCoord) * (dispStrength * 0.4);

//     // Combine all displacement effects
//     float finalDisplacement = baseLayer + poolLayer + waveLayer;

//     // Enhanced water colors
//     // vec3 deepColor = vec3(0.0, 0.2, 0.5);    // Darker blue for deep water
//     // vec3 shallowColor = vec3(0.2, 0.5, 0.8); // Lighter blue for shallow water

//     // Water colors with less contrast
//     vec3 deepColor = vec3(0.2, 0.4, 0.8);    // Make deep water lighter
//     vec3 shallowColor = vec3(0.2, 0.4, 0.8); // Make shallow water closer to deep color


//     vec3 foamColor = vec3(0.8, 0.9, 1.0);    // Almost white for foam/waves

//     // Calculate threshold effect
//     float threshold = 0.4;
//     float poolEffect = smoothstep(threshold - 0.1, threshold + 0.1, abs(finalDisplacement));

//     // Mix colors based on displacement and threshold
//     vec3 baseColor = mix(deepColor, shallowColor, abs(finalDisplacement));
//     vec3 finalColor = mix(baseColor, foamColor, poolEffect);

//     // Enhanced edge fade calculation
//     vec2 center = vec2(0.5, 0.5);
//     vec2 pos = TexCoord - center;
//     float ellipticalDist = length(vec2(pos.x * 1.5, pos.y * 2.0));

//     // Combine edge fade with wave effect
//     // float edgeFade = smoothstep(0.8, 0.3, ellipticalDist);
//     // Adjust edge fade to be more subtle
//     float edgeFade = smoothstep(0.9, 0.4, ellipticalDist); // Wider transition range

//     float waveEdge = sin(ellipticalDist * 20.0 + time * 2.0) * 0.5 + 0.5;
//     float finalAlpha = edgeFade * (0.8 + waveEdge * 0.2);

//     // Output final color with transparency
//     FragColor = vec4(finalColor, finalAlpha * 0.9);
// }


// #version 330 core
// out vec4 FragColor;

// in vec2 TexCoord;
// in vec2 DispCoord1;
// in vec2 DispCoord2;
// in vec2 WaveCoord;

// uniform sampler2D dispTexture;
// uniform float dispStrength;
// uniform float time;

// // Helper function to process water layers
// float processWaterLayer(vec2 coord) {
//     // Get raw value from displacement map
//     vec2 dispValue = texture(dispTexture, coord).rg;
//     // Convert to -1 to 1 range and enhance effect
//     return (length(dispValue) * 2.0 - 1.0);
// }

// void main() {
//     // Process three separate layers with adjusted strengths
//     float baseLayer = processWaterLayer(DispCoord1) * dispStrength;
//     float poolLayer = processWaterLayer(DispCoord2) * (dispStrength * 0.7);
//     float waveLayer = processWaterLayer(WaveCoord) * (dispStrength * 0.4);

//     // Combine all displacement effects
//     float finalDisplacement = baseLayer + poolLayer + waveLayer;

//     // Single base color for water
//     vec3 waterColor = vec3(0.1, 0.3, 0.7);

//     // Add slight variation based on waves
//     vec3 finalColor = waterColor + vec3(finalDisplacement * 0.1);

//     // Calculate simple edge fade
//     vec2 center = vec2(0.5, 0.5);
//     vec2 pos = TexCoord - center;
//     float dist = length(vec2(pos.x * 1.5, pos.y * 2.0));

//     // Simple fade out at edges
//     float alpha = smoothstep(0.8, 0.3, dist);

//     // Output final color
//     FragColor = vec4(finalColor, alpha * 0.9);
// }

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 DispCoord1;
in vec2 DispCoord2;
in vec2 WaveCoord;

uniform sampler2D dispTexture;
uniform float dispStrength;
uniform float time;

float processWaterLayer(vec2 coord) {
    vec2 dispValue = texture(dispTexture, coord).rg;
    return (length(dispValue) * 2.0 - 1.0);
}

void main() {
    // Process displacement layers
    float baseLayer = processWaterLayer(DispCoord1) * dispStrength;
    float poolLayer = processWaterLayer(DispCoord2) * (dispStrength * 0.7);
    float waveLayer = processWaterLayer(WaveCoord) * (dispStrength * 0.4);

    float finalDisplacement = baseLayer + poolLayer + waveLayer;

    // Base water color
    vec3 waterColor = vec3(0.1, 0.3, 0.7);

    // Add very subtle variation from displacement
    vec3 finalColor = waterColor + waterColor * (finalDisplacement * 0.1);

    // Simple edge fade
    vec2 center = vec2(0.5, 0.5);
    vec2 pos = TexCoord - center;
    float dist = length(vec2(pos.x * 1.5, pos.y * 2.0));
    float alpha = smoothstep(0.8, 0.5, dist) * 0.9;

    // Add subtle wave effect to alpha
    alpha *= (1.0 + sin(dist * 10.0 + time) * 0.1);

    FragColor = vec4(finalColor, alpha);
}
