#pragma once

#include <vector>
#include "glm/glm.hpp"

class TerrainGenerator
{
public:
    bool m_wireshade;

    // Make these constants static public
    static const float CHUNK_SIZE;    // Size of each chunk in world units
    static const float VERTEX_SPACING; // Distance between vertices

    TerrainGenerator();
    ~TerrainGenerator();
    int getResolution() { return m_resolution; };
    std::vector<float> generateTerrain();
    std::vector<float> generateTerrainChunk(int chunkX, int chunkZ);
    float getWorldHeight(float worldX, float worldZ);

private:

    // Member variables for terrain generation. You will not need to use these directly.

    // const float CHUNK_SIZE = 25.0f; // Size of each chunk in world units
    // const float VERTEX_SPACING = 0.5f; // Distance between vertices

    glm::vec2 worldToLocal(float worldX, float worldZ);
    glm::vec2 localToWorld(float localX, float localZ, int chunkX, int chunkZ);

    std::vector<glm::vec2> m_randVecLookup;
    int m_resolution;
    int m_lookupSize;

    // Samples the (infinite) random vector grid at (row, col)
    glm::vec2 sampleRandomVector(int row, int col);

    // Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
    // Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
    glm::vec3 getPosition(int row, int col);

    // ================== Students, please focus on the code below this point

    // Takes a normalized (x, y) position, in range [0,1)
    // Returns a height value, z, by sampling a noise function
    float getHeight(float x, float y);

    // Computes the normal of a vertex by averaging neighbors
    glm::vec3 getNormal(int row, int col);

    // Computes color of vertex using normal and, optionally, position
    glm::vec3 getColor(glm::vec3 normal, glm::vec3 position);

    // Computes the intensity of Perlin noise at some point
    float computePerlin(float x, float y);
};
