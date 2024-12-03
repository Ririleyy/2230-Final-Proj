#version 330 core

const float M_PI = 3.1415926535897932384626433832795;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 uvPos;


uniform mat4 pvm, model, normalMatrix;

void main() {
    uvPos = uv;
    worldPos = vec3(model * vec4(position, 1.0));
    worldNormal = normalize(vec3(normalMatrix * vec4(normalize(normal), 0.0)));
    
    gl_Position = pvm * vec4(position, 1.0);
}
