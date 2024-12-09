#version 330 core

in vec2 fragUV; // UV coordinates from vertex shader
uniform sampler2D texture1; // Texture sampler
uniform sampler2D texture2;
uniform int activeTexture;
uniform float alpha; // Alpha value for fading

out vec4 fragColor;

void main() {
<<<<<<< Updated upstream
    vec3 texColor = texture(texture1, fragUV).rgb; // Sample the texture using UV coordinates
    fragColor = vec4(texColor, 1.0); // Output the texture color
=======
    vec4 texColor; // Sample the texture using UV coordinates
    //float finalHeight = clamp(height / 100, 0.0, 1.0); // Clamp the height between 0 and 1
    //fragColor = vec4(finalHeight, finalHeight, finalHeight, 1.0); // Output the texture color

    if(activeTexture == 0){
        texColor = texture(texture1, fragUV);
    }else{
        texColor = texture(texture2, fragUV);
    }
    fragColor = vec4(texColor.rgb, texColor.a * alpha);
>>>>>>> Stashed changes

}
