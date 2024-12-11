#version 330 core

in vec2 fragUV;
in vec3 outColor;
in float height;
in vec3 fragNormal;

uniform sampler2D texture1; // snow
uniform sampler2D texture2; // rock
uniform sampler2D texture3; // grass
uniform sampler2D texture4; // sand
uniform sampler2D texture5; // water
uniform int activeTexture;
uniform float brightness;
uniform float minBrightness;
uniform float transitionWidth;

out vec4 fragColor;

// Improved transition function for smoother blending
float getTransitionFactor(float value, float threshold, float width) {
    float lowerBound = threshold - width;
    float upperBound = threshold + width;

    if(value <= lowerBound) return 0.0;
    if(value >= upperBound) return 1.0;

    float t = (value - lowerBound) / (upperBound - lowerBound);
    // Using improved smoothstep for better transition
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

void main() {
    // Get base texture colors with adjusted UV scaling for better detail
    vec4 waterColor = texture(texture5, fragUV * 2.0);
    vec4 sandColor = texture(texture4, fragUV * 3.0); // Increased sand texture detail
    vec4 mountainColor;

    // Select and scale mountain texture
    if(activeTexture == 0) {
        mountainColor = texture(texture1, fragUV * 2.0);
    } else if(activeTexture == 1) {
        mountainColor = texture(texture2, fragUV * 2.0);
    } else {
        mountainColor = texture(texture3, fragUV * 2.0);
    }

    vec4 finalColor;

    // Enhanced beach area transition
    float sandFactor = getTransitionFactor(height, 0.02, transitionWidth * 1.5);
    // Wider transition zone for beach
    if (height < 0.05) { // Expanded beach zone
        float waterBlend = getTransitionFactor(height, 0.01, transitionWidth);
        finalColor = mix(waterColor, sandColor, waterBlend);
        // Enhance sand color visibility
        finalColor = mix(finalColor, sandColor, sandFactor * 1.2);
    } else {
        float mountainBlend = getTransitionFactor(height, 0.1, transitionWidth);
        finalColor = mix(sandColor, mountainColor, mountainBlend);
    }

    // Enhance brightness for beach areas
    float beachBrightness = 1.0 + (sandFactor * 0.2); // Slight brightness boost for beach
    float effectiveBrightness = max(brightness * beachBrightness, minBrightness);

    // Output final color with enhanced alpha for beach areas
    fragColor = vec4(finalColor.rgb * effectiveBrightness, finalColor.a);
}
