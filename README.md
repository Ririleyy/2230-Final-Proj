### Parameters to Adjust for Running Effects
# Debug Parameters Guide

This document lists key parameters that can be adjusted for debugging and tuning the scene rendering.

```glsl
// In water.frag
// Base Water Color
vec3 waterColor = vec3(0.1, 0.3, 0.7);  // Adjust these three values to change the water color
// Effect of Ripples on Color
vec3 finalColor = waterColor + waterColor * (finalDisplacement * 0.1);  // Adjust this 0.1 to change the prominence of ripples
// Transparency Control
float alpha = smoothstep(0.8, 0.5, dist) * 0.9;  // Adjust these values to change the edge gradient range and overall transparency

// In water.vert
// Speed of Different Wave Layers
DispCoord1 = aTexCoord + vec2(time * 0.1, time * 0.08);    // Adjust these values to change the speed of the first wave layer
DispCoord2 = aTexCoord + vec2(-time * 0.05, time * 0.07);  // Adjust these values to change the speed of the second wave layer
WaveCoord = aTexCoord * 2.0 + vec2(time * 0.15);          // Adjust these values to change the speed of the third wave layer




## Terrain Parameters
Located in `src/utils/terrain.h` and `src/utils/terrain.cpp`:
```cpp
// Terrain chunk size and vertex spacing
static const float CHUNK_SIZE = 25.0f;      // Size of each terrain chunk
static const float VERTEX_SPACING = 0.5f;    // Distance between vertices in terrain mesh

// In getWorldHeight():
float scaledX = worldX * 0.02f;             // Scale factor for terrain coordinate X
float scaledZ = worldZ * 0.02f;             // Scale factor for terrain coordinate Z
float height = getHeight(scaledX, scaledZ) * 50.0f;  // Height multiplier for terrain
```

## Rendering Distance Parameters
Located in `src/glrenderer.h`:
```cpp
static const int RENDER_DISTANCE = 5;        // Controls terrain render distance
static const int WATER_RENDER_DISTANCE = 2;  // Controls water plane render distance
```

## Water Parameters
Located in `src/glrenderer.h` and `src/glrenderer.cpp`:
```cpp
float m_waterLevel = 0.0f;    // Base water level height

// In createWaterPlane():
float checkStep = TerrainGenerator::CHUNK_SIZE / 4.0f;  // Sampling density for water plane generation
if (height < m_waterLevel + 5.0f) { ... }    // Height threshold for water plane generation
if (belowWaterPoints >= checkPoints * 0.3f) { ... }  // Percentage threshold for water plane creation
```

## Camera Parameters
Located in `src/glrenderer.h`:
```cpp
const float m_rotSpeed = 0.005;     // Camera rotation speed
const float m_translSpeed = 25;     // Camera movement speed
const float m_maxHeight = 45.0f;    // Maximum camera height
const float m_minHeight = 5.0f;     // Minimum camera height
```

## Effects and Transition Parameters
Located in `src/glrenderer.cpp`:
```cpp
float fadeDuration = 2000.0f;  // Duration for fade in/out effects in milliseconds
```

## Adjustment Tips
- To adjust terrain height: Modify the height multiplier in `getWorldHeight()`
- To change terrain density: Adjust `VERTEX_SPACING`
- To modify view distance: Adjust `RENDER_DISTANCE` and `WATER_RENDER_DISTANCE`
- To change water coverage: Modify the threshold in createWaterPlane() (0.3f)
- To adjust camera movement: Modify `m_rotSpeed` and `m_translSpeed`
