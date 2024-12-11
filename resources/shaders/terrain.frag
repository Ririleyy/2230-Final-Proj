#version 330 core

in vec2 fragUV; // UV coordinates from vertex shader
in vec3 outColor;
uniform sampler2D texture1; // snow
uniform sampler2D texture2; // rock
uniform sampler2D texture3; // grass
uniform sampler2D texture4; // sand
uniform sampler2D texture5; // water
uniform int activeTexture;
uniform float alpha; // Alpha value for fading
uniform float brightness; // Dynamic brightness factor (0.0 to 1.0)
uniform float minBrightness; // Minimum brightness value (e.g., 0.3)

out vec4 fragColor;

void main() {
    vec4 texColor;

    // Check if outColor corresponds to grass color (0.0, 1.0, 0.0)
    if (outColor == vec3(0.0, 0.0, 0.0)) {
        texColor = texture(texture5, fragUV); // Use seafloor texture
    } else if(outColor == vec3(1.0, 0.0, 0.0)){

        texColor = texture(texture4, fragUV); // sand texture
    }else{
        if(activeTexture == 0){
            texColor = texture(texture1, fragUV);
        }else if(activeTexture == 1){
            texColor = texture(texture2, fragUV);
        }else if(activeTexture == 2){
            texColor = texture(texture3, fragUV);
        }
    }

    float effectiveBrightness = max(brightness, minBrightness);

    // Apply brightness adjustment to the sampled texture color
    vec3 adjustedColor = texColor.rgb * effectiveBrightness;

    // Apply alpha blending and output the final color
    fragColor = vec4(adjustedColor, texColor.a * alpha);


}

// #version 330 core

// in vec2 fragUV; // UV coordinates from vertex shader
// in vec3 outColor;
// uniform sampler2D texture1; // snow
// uniform sampler2D texture2; // rock
// uniform sampler2D texture3; // grass
// uniform sampler2D texture4; // sand
// uniform sampler2D texture5; // water
// uniform int activeTexture;
// uniform float alpha; // Alpha value for fading
// uniform float brightness; // Dynamic brightness factor (0.0 to 1.0)
// uniform float minBrightness; // Minimum brightness value (e.g., 0.3)

// out vec4 fragColor;

// void main() {
//     vec4 texColor;
//     if (outColor == vec3(0.0, 0.0, 0.0)) {
//         texColor = texture(texture5, fragUV); // Use seafloor texture
//     }else if(outColor == vec3(1.0, 0.0, 0.0)){

//         texColor = texture(texture4, fragUV);

//     }else{

//         if(activeTexture == 0){

//             texColor = texture(texture1, fragUV);

//         }else if(activeTexture == 1){

//             texColor = texture(texture2, fragUV);

//         }else if(activeTexture == 2){

//             texColor = texture(texture3, fragUV);

//         }

//     }


//     float effectiveBrightness = max(brightness, minBrightness);

//     // Apply brightness adjustment to the sampled texture color
//     vec3 adjustedColor = texColor.rgb * effectiveBrightness;

//     // Apply alpha blending and output the final color
//     fragColor = vec4(adjustedColor, texColor.a * alpha);


// }

