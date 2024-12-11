#version 330 core

in vec2 fragUV;
in vec3 outColor;
in float height;
in vec3 fragNormal;

uniform sampler2D texture1; // snow middle
uniform sampler2D texture2; // rock middle + top
uniform sampler2D texture3; // grass middle
uniform sampler2D texture4; // grass and rock's sand
uniform sampler2D texture5; // water
uniform sampler2D texture6; // snowTop
uniform sampler2D texture7; // snowSand
uniform sampler2D texture8; // grassTop
uniform sampler2D texture9; // rockSand
uniform sampler2D texture10; // seafloor
uniform sampler2D texture11; // rockMiddle
uniform int activeTexture;
uniform float brightness;
uniform float minBrightness;
uniform float transitionWidth;

out vec4 fragColor;

// Improved transition function for smoother blending
float getTransitionFactor(float value, float threshold, float width) {
    float lowerBound = threshold - width;
    float upperBound = threshold + width;

    if (value <= lowerBound) return 0.0;
    if (value >= upperBound) return 1.0;

    float t = (value - lowerBound) / (upperBound - lowerBound);
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

void main() {
    vec4 seaFloor = texture(texture10, fragUV * 2.0); // Adjusted for detail
    vec4 sandColor = texture(texture4, fragUV * 3.0);  // Increased sand detail
    vec4 middleColor, topColor, sandLayerColor;

    // Determine active mountain type and corresponding layers
    if (activeTexture == 0) { // Snow mountain
        middleColor = texture(texture1, fragUV * 2.0);
        // Increase brightness for the snow middle layer
        float snowBrightnessFactor = 1.2; // Increase brightness by 50%
        middleColor.rgb = clamp(middleColor.rgb * snowBrightnessFactor, 0.0, 1.0);

        middleColor = texture(texture9, fragUV * 2.0);

        float yellowFactor = smoothstep(5.0, 20.0, height); // Scale yellow tint based on height
        vec3 yellowTint = vec3(0.7, 0.6, 0.2) * yellowFactor; // Strong yellow tint scaling with height
        middleColor.rgb = clamp(middleColor.rgb + yellowTint, 0.0, 1.0);

        topColor = texture(texture6, fragUV * 2.0);
        sandLayerColor = texture(texture7, fragUV * 3.0);
    } else if (activeTexture == 1) { // Rock mountain
        // middleColor = texture(texture9, fragUV * 2.0);

        // float yellowFactor = smoothstep(5.0, 20.0, height); // Scale yellow tint based on height
        // vec3 yellowTint = vec3(0.7, 0.6, 0.2) * yellowFactor; // Strong yellow tint scaling with height
        // middleColor.rgb = clamp(middleColor.rgb + yellowTint, 0.0, 1.0);
        middleColor = texture(texture3, fragUV * 2.0);



        topColor = texture(texture2, fragUV * 2.0); // Rock uses the same texture for middle and top
        sandLayerColor = texture(texture11, fragUV * 3.0);
    } else { // Grass mountain
        middleColor = texture(texture3, fragUV * 2.0);

        topColor = texture(texture8, fragUV * 2.0);
        float grassBrightnessFactor = 1.2; // Slightly increase brightness
        vec3 grassTint = vec3(0.0, 0.05, 0.0); // Add green tint
        topColor.rgb = clamp(topColor.rgb * grassBrightnessFactor + grassTint, 0.0, 1.0);

        sandLayerColor = texture(texture4, fragUV * 3.0);
    }

    vec4 mountainColor;

    // Blend between layers based on height
    float sandFactor = getTransitionFactor(height, 0.1, transitionWidth * 2);
    float middleFactor = getTransitionFactor(height, 0.3, transitionWidth *5);
    float topFactor = getTransitionFactor(height, 3, transitionWidth * 10);

    if (height < 0.1) {
        mountainColor = mix(seaFloor, sandLayerColor, sandFactor);
        //mountainColor = vec4(0.0);
    } else if (height < 2) {
        mountainColor = mix(sandLayerColor, middleColor, middleFactor);
    } else {
        mountainColor = mix(middleColor, topColor, topFactor);
    }

    // Adjust brightness
    float effectiveBrightness = max(brightness, minBrightness);
    mountainColor.rgb *= effectiveBrightness;

    // Final color with alpha
    fragColor = vec4(mountainColor.rgb, mountainColor.a);
}


