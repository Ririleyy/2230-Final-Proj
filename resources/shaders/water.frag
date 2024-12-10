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
    // Calculate water displacement
    float baseLayer = processWaterLayer(DispCoord1) * dispStrength;
    float poolLayer = processWaterLayer(DispCoord2) * (dispStrength * 0.7);
    float waveLayer = processWaterLayer(WaveCoord) * (dispStrength * 0.4);
    float finalDisplacement = baseLayer + poolLayer + waveLayer;

    // Water color calculation
    vec3 waterColor = vec3(0.1, 0.3, 0.7);
    vec3 finalColor = waterColor + waterColor * (finalDisplacement * 0.1);

    // Calculate alpha
    float alpha = 0.9;

    FragColor = vec4(finalColor, alpha);
}
