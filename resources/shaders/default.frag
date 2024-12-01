#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 worldPos;
in vec3 worldNormal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float k_a;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float k_d;
uniform vec4 lightPos;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float k_s;
uniform float shininess;
uniform vec4 camPos;

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    // fragColor = vec4(1.0);

    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    fragColor = vec4(abs(worldNormal), 1.0);

    // Task 12: add ambient component to output color
    // fragColor = vec4(k_a, k_a, k_a, 1.0);
    // Task 13: add diffuse component to output color
    // vec3 L = normalize(lightPos.xyz - worldPos);
    // vec3 N = normalize(worldNormal);
    // float diffuse = clamp(dot(N, L), 0.0, 1.0);
    // fragColor += vec4(k_d * diffuse, k_d * diffuse, k_d * diffuse, 1.0);

    // Task 14: add specular component to output color
    // vec3 R = reflect(-L, N);
    // vec3 E = normalize(camPos.xyz - worldPos);
    // fragColor += vec4(k_s * specular, k_s * specular, k_s * specular, 1.0);
    // float specular = pow(clamp(dot(R, E), 0.0, 1.0), shininess);
}
