#pragma once
#include <vector>
#include "glm/glm.hpp"

class TerrainGenerator {
public:
    static const float CHUNK_SIZE;
    static const float VERTEX_SPACING;

    TerrainGenerator();
    ~TerrainGenerator();
    int getResolution() { return m_resolution; };
    std::vector<float> generateTerrain();
    std::vector<float> generateTerrainChunk(int chunkX, int chunkZ);
    float getWorldHeight(float worldX, float worldZ);

private:
    // Basic terrain parameters
    std::vector<glm::vec2> m_randVecLookup;
    int m_resolution;
    int m_lookupSize;
    const int m_scale = 120;

    // Adjust terrain level thresholds
    const float m_waterLevel = 0.0001f;  // Keeps water level low
    const float m_sandLevel = 0.05f;     // Increased sand level range
    const float m_grassLevel = 0.2f;     // Pushed up grass level
    const float m_rockLevel = 0.35f;     // Pushed up rock level

    // Widen transition zones for smoother blending
    const float m_transitionWidth = 0.03f;  // Increased for smoother transitions
    const float m_waterTransition = 0.04f;  // Wider water transition
    const float m_sandTransition = 0.06f;   // Wider sand transition
    const float m_grassTransition = 0.08f;  // Wider grass transition

    // Helper functions
    glm::vec2 sampleRandomVector(int row, int col);
    glm::vec3 getPosition(int row, int col);
    float getHeight(float x, float y);
    glm::vec3 getNormal(int row, int col);
    glm::vec3 getColor(float worldX, float worldZ);
    float computePerlin(float x, float y);
    float mapHeight(float normalizedHeight);
    glm::vec2 worldToLocal(float worldX, float worldZ);
    glm::vec2 localToWorld(float localX, float localZ, int chunkX, int chunkZ);
};
