// #version 330 core
// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 normal;
// layout(location = 2) in vec3 color;
// layout(location = 3) in vec2 uv;
// out vec3 outColor;
// out vec2 fragUV;
// out float height;


// uniform mat4 projection;
// uniform mat4 model;
// uniform mat4 view;


// void main() {
//     gl_Position = projection * view * model * vec4(position, 1.0);
//     height = position.y;
//     outColor = color;
//     fragUV = uv; // Pass UV to fragment shader
// }

#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

out vec3 outColor;  // Pass vertex color to fragment shader
out vec2 fragUV;    // Pass UV coordinates to fragment shader
out float height;   // Pass normalized height to fragment shader
out vec3 fragNormal; // Pass world-space normal to fragment shader

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
    // Transform vertex position to clip space
    gl_Position = projection * view * model * vec4(position, 1.0);

    // Calculate normalized height in world space (assuming y is the height axis)
    vec4 worldPosition = model * vec4(position, 1.0);
    height = worldPosition.y;  // Use actual y-coordinate for height

    // Pass other attributes to fragment shader
    outColor = color;
    fragUV = uv;
    fragNormal = mat3(transpose(inverse(model))) * normal; // Transform normal to world space
}

