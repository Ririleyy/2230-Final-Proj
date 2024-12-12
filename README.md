# Nature Werks

A real-time natural environment renderer built with OpenGL, featuring procedurally generated infinite terrain, dynamic weather system, physically-based sky rendering, and animated water effects.

## Features

### Atmospheric Scattering
- Physical sky rendering based on the Preetham Sky Model
- Dynamic sun position and lighting changes
- Turbidity adjustment based on weather conditions

### Rippling Waters
- Multi-layered displacement mapping for water animation
- Dynamic UV coordinate transformation
- Transparency-based water rendering
- Dynamically generated water system following terrain

### Infinite Terrain
- Multi-threaded terrain generation
- Perlin noise-based height generation
- Three terrain types:
  - Snowy Mountains
  - Rocky Mountains
  - Grass Mountains
- Height-based texture layering:
  - Seafloor
  - Sand
  - Middle layer
  - Top layer
- Smooth height and texture transitions

### Dynamic Weather
- GPU-based particle system
- Three weather conditions:
  - Clear
  - Rain
  - Snow
- Particle physics simulation with gravity and wind effects

## Controls

### Camera Controls
- WASD: Move forward/left/backward/right
- Space/Ctrl: Move up/down
- Left Mouse Button: Rotate view
- Right Mouse Button: Orbit around point
- R: Toggle auto-rotation
- Mouse Wheel: Zoom in/out

### Scene Controls
- FOV Slider: Adjust field of view (10° - 179°)
- Time Slider: Control time of day (0-24)
- Weather Toggle: Switch between Clear/Rain/Snow
- Terrain Toggle: Switch between Snow/Rock/Grass mountains

## System Requirements
- OS: Any operating system supporting OpenGL 4.1
- GPU: Dedicated graphics card with OpenGL 4.1 support
- CPU: Modern processor supporting multi-threading
- Memory: 4GB RAM minimum
- Recommended Resolution: 1920x1080 or higher

## Dependencies
- Qt 6.0+
- OpenGL 4.1+
- GLEW
- GLM

## Known Issues
1. Terrain Generation
- Brief loading delay during rapid camera movement
- Visible seams at terrain chunk boundaries at extreme angles

2. Rendering
- Particle effects may impact performance on lower-end systems
- Water transparency artifacts from certain viewing angles

3. Camera
- Rendering artifacts at extreme pitch angles

## Performance Tips
1. Adjust render distance for better performance
2. Disable weather effects on lower-end systems
3. Adjust FOV to balance between visibility and performance
4. Consider terrain generation queue size based on system capabilities

## Team
Team Nature Werks:
Tianhao Shi
Xiaoxi Yang
Jiayi Li
Haoyang Li

## Collaboration/References

Used ChatGPT for:
Looking up OpenGL function syntax and usage
Understanding mathematical concepts
Code review and identifying potential edge cases
Code organization and OOP design patterns
Debugging error messages and syntax errors
Code quality improvements (conciseness, readability)
Function parameter passing best practices

---
Note: This project was developed as part of CSCI 1230.
