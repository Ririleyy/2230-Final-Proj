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

out vec4 fragColor;

void main() {
    vec4 texColor;

    // Check if outColor corresponds to grass color (0.0, 1.0, 0.0)
    if (outColor == vec3(0.0, 0.0, 0.0)) {
        texColor = texture(texture5, fragUV); // Use seafloor texture
    } else if(outColor == vec3(1.0, 0.0, 0.0)){

        texColor = texture(texture4, fragUV); // sand texture
    }else if(outColor == vec3(0.0, 1.0, 0.0)){

        texColor = texture(texture3, fragUV); // grass texture
    }else if(outColor == vec3(0.0, 0.0, 1.0)){

        texColor = texture(texture2, fragUV); // rock texture
    }else if(outColor == vec3(1.0, 1.0, 1.0)){

        texColor = texture(texture1, fragUV); // snow texture
    }


    fragColor = vec4(texColor.rgb, texColor.a * alpha);


}
