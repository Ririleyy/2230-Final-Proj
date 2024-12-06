#version 330 core
in float life;
out vec4 FragColor;
uniform bool isSnow;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float circle = dot(circCoord, circCoord);

    if (circle > 1.0) {
        discard;
    }

    if (isSnow) {
        vec3 snowColor = vec3(1.0, 1.0, 1.0);
        float alpha = life * (1.0 - circle * 0.5);
        FragColor = vec4(snowColor, alpha);
    } else {
        vec3 rainColor = vec3(0.7, 0.85, 1.0);  // More blue tint
        float alpha = life * (1.0 - circle * 0.3);  // Increased opacity
        FragColor = vec4(rainColor, alpha * 0.8);  // Increased overall opacity
    }
}
