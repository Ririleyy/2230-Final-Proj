#version 330 core

in vec2 fragUV; // UV coordinates from vertex shader
in vec3 outColor;
uniform sampler2D texture1; // Texture sampler
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform int activeTexture;
uniform float alpha; // Alpha value for fading

out vec4 fragColor;

void main() {
    fragColor = vec4(outColor, 1.0);

    // vec4 texColor; // Sample the texture using UV coordinates
    // //float finalHeight = clamp(height / 100, 0.0, 1.0); // Clamp the height between 0 and 1
    // //fragColor = vec4(finalHeight, finalHeight, finalHeight, 1.0); // Output the texture color

    // if(activeTexture == 0){
    //     texColor = texture(texture1, fragUV);
    // }else if(activeTexture == 1) {
    //     texColor = texture(texture2, fragUV);
    // }else if(activeTexture == 2){
    //     texColor = texture(texture3, fragUV);
    // }

    // fragColor = vec4(texColor.rgb, texColor.a * alpha);


}
