### Parameters to Adjust for Running Effects

```glsl
// In water.frag
// Base Water Color
vec3 waterColor = vec3(0.1, 0.3, 0.7);  // Adjust these three values to change the water color
// Effect of Ripples on Color
vec3 finalColor = waterColor + waterColor * (finalDisplacement * 0.1);  // Adjust this 0.1 to change the prominence of ripples
// Transparency Control
float alpha = smoothstep(0.8, 0.5, dist) * 0.9;  // Adjust these values to change the edge gradient range and overall transparency

// In the paintWater() function
// Wave Speed
m_water_time += 0.01f;  // Adjust this value to change the wave speed
// Wave Intensity
glUniform1f(glGetUniformLocation(m_water_shader, "dispStrength"), 0.5f);  // Adjust this value to change the wave intensity

// In water.vert
// Speed of Different Wave Layers
DispCoord1 = aTexCoord + vec2(time * 0.1, time * 0.08);    // Adjust these values to change the speed of the first wave layer
DispCoord2 = aTexCoord + vec2(-time * 0.05, time * 0.07);  // Adjust these values to change the speed of the second wave layer
WaveCoord = aTexCoord * 2.0 + vec2(time * 0.15);          // Adjust these values to change the speed of the third wave layer
