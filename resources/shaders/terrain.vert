#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;
out vec3 fragColor;
out vec2 fragUV;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;



void main() {
    gl_Position = proj * view * model * vec4(position, 1.0);
    fragColor = color;
    fragUV = uv; // Pass UV to fragment shader
}


