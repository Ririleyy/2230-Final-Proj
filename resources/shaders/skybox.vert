#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 position;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 worldPos;
out vec3 worldNormal;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 model;
// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5
    worldPos = vec3(model * vec4(position, 1.0));
    mat4 normalMatrix = transpose(inverse(model));
    worldNormal = normalize(vec3(normalMatrix * vec4(normalize(position), 0.0)));
    
    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.


    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = projection * view * model * vec4(position, 1.0);
}
