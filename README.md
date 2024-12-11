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
  


# Terrain Generation System Documentation

## Overview
This documentation explains how to modify the terrain generation system, particularly focusing on terrain layering and texture transitions.

## Key Components

### Terrain Height Thresholds
Located in `terrain.h`, these constants control the height levels for different terrain types:
```cpp
const float m_waterLevel = 0.0001f;  // Water surface level
const float m_sandLevel = 0.05f;     // Beach/sand level
const float m_grassLevel = 0.2f;     // Grass/vegetation level
const float m_rockLevel = 0.35f;     // Rock/mountain level
```

### Transition Parameters
Controls the smoothness of transitions between different terrain types:
```cpp
const float m_transitionWidth = 0.03f;  // Global transition width
const float m_waterTransition = 0.04f;  // Water to sand transition
const float m_sandTransition = 0.06f;   // Sand to grass transition
const float m_grassTransition = 0.08f;  // Grass to rock transition
```

## How to Modify Terrain Layers

### 1. Adjusting Layer Heights
- Increase `m_sandLevel` to create larger beach areas
- Modify `m_grassLevel` to change where vegetation starts
- Adjust `m_rockLevel` to control mountain height distribution

### 2. Smoothing Transitions
- Increase transition values for smoother blending between layers
- Decrease for more distinct boundaries
- Use values between 0.02 and 0.1 for best results

### 3. Shader Customization
In `terrain.frag`:
- Modify `getTransitionFactor()` for different transition styles
- Adjust texture tiling using UV scaling:
```glsl
vec4 sandColor = texture(texture4, fragUV * 3.0); // Adjust scale factor
```

## Example Configurations

### Mountain-Heavy Terrain
```cpp
const float m_waterLevel = 0.0001f;
const float m_sandLevel = 0.03f;    // Reduced beach area
const float m_grassLevel = 0.15f;   // Lower vegetation
const float m_rockLevel = 0.25f;    // More mountain coverage
```

### Beach-Heavy Terrain
```cpp
const float m_waterLevel = 0.0001f;
const float m_sandLevel = 0.08f;    // Extended beach area
const float m_grassLevel = 0.25f;   // Higher vegetation start
const float m_rockLevel = 0.4f;     // Reduced mountain area
```

## Tips
1. Adjust values incrementally (0.01-0.05 steps)
2. Keep transition widths smaller than the distance between levels
3. Test changes with different viewing angles
4. Consider performance impact when using very wide transitions

## Common Issues
1. **Sharp Transitions**: Increase relevant transition width
2. **Blurry Textures**: Adjust UV scaling in fragment shader
3. **Missing Layers**: Check if height thresholds overlap
4. **Performance Issues**: Reduce transition widths

## Performance Considerations
- Wider transitions require more texture blending
- More distinct layers (smaller transitions) perform better
- Balance visual quality with performance needs



