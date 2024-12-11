#include "terrain.h"
#include <cmath>
#include "glm/glm.hpp"
#include <iostream>

// Initialize static members
const float TerrainGenerator::CHUNK_SIZE = 30.0f;
const float TerrainGenerator::VERTEX_SPACING = 0.5f;

// Constructor
TerrainGenerator::TerrainGenerator() {
    m_resolution = 100;
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);
    std::srand(1230);

    for (int i = 0; i < m_lookupSize; i++) {
        m_randVecLookup.push_back(glm::vec2(
            std::rand() * 2.0 / RAND_MAX - 1.0,
            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

TerrainGenerator::~TerrainGenerator() {
    m_randVecLookup.clear();
}

void addPointToVector(glm::vec3 point, glm::vec3 normal, glm::vec3 color, glm::vec2 uv, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.y);
    vector.push_back(point.z);
    vector.push_back(normal.x);
    vector.push_back(normal.y);
    vector.push_back(normal.z);
    vector.push_back(color.r);
    vector.push_back(color.g);
    vector.push_back(color.b);
    vector.push_back(uv.x);
    vector.push_back(uv.y);
}

glm::vec2 calculateUV(int x, int y, int resolution) {
    float u = static_cast<float>(x) / static_cast<float>(resolution - 1);
    float v = static_cast<float>(y) / static_cast<float>(resolution - 1);
    return glm::vec2(glm::clamp(u, 0.0f, 1.0f), glm::clamp(v, 0.0f, 1.0f));
}

std::vector<float> TerrainGenerator::generateTerrain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for (int x = 0; x < m_resolution; x++) {
        for (int y = 0; y < m_resolution; y++) {
            int x1 = x, y1 = y;
            int x2 = x + 1, y2 = y + 1;

            glm::vec3 p1 = getPosition(x1, y1);
            glm::vec3 p2 = getPosition(x2, y1);
            glm::vec3 p3 = getPosition(x2, y2);
            glm::vec3 p4 = getPosition(x1, y2);

            glm::vec3 n1 = getNormal(x1, y1);
            glm::vec3 n2 = getNormal(x2, y1);
            glm::vec3 n3 = getNormal(x2, y2);
            glm::vec3 n4 = getNormal(x1, y2);

            glm::vec3 c1 = getColor(p1.x, p1.z);
            glm::vec3 c2 = getColor(p2.x, p2.z);
            glm::vec3 c3 = getColor(p3.x, p3.z);
            glm::vec3 c4 = getColor(p4.x, p4.z);

            glm::vec2 uv1 = calculateUV(x1, y1, m_resolution);
            glm::vec2 uv2 = calculateUV(x2, y1, m_resolution);
            glm::vec2 uv3 = calculateUV(x2, y2, m_resolution);
            glm::vec2 uv4 = calculateUV(x1, y2, m_resolution);

            // Triangle 1
            addPointToVector(p1, n1, c1, uv1, verts);
            addPointToVector(p2, n2, c2, uv2, verts);
            addPointToVector(p3, n3, c3, uv3, verts);

            // Triangle 2
            addPointToVector(p1, n1, c1, uv1, verts);
            addPointToVector(p3, n3, c3, uv3, verts);
            addPointToVector(p4, n4, c4, uv4, verts);
        }
    }
    return verts;
}

glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col) {
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    float x = 1.0 * row / m_resolution;
    float y = 1.0 * col / m_resolution;
    float z = getHeight(x, y);
    return glm::vec3(x, y, z);
}

float interpolate(float A, float B, float alpha) {
    float alpha_ease = 3 * alpha * alpha - 2 * alpha * alpha * alpha;
    return A + alpha_ease * (B - A);
}

float TerrainGenerator::getHeight(float x, float y) {
    float total = 0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxAmplitude = 0;

    for (int i = 0; i < 6; i++) {
        total += computePerlin(x * frequency, y * frequency) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= 0.6;
        frequency *= 2.0;
    }
    return total / maxAmplitude;
}

glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    glm::vec3 normal(0, 0, 0);
    std::vector<std::vector<int>> neighborOffsets = {
        {-1, -1}, { 0, -1}, { 1, -1},
        { 1,  0}, { 1,  1}, { 0,  1},
        {-1,  1}, {-1,  0}
    };

    glm::vec3 V = getPosition(row, col);
    for (int i = 0; i < 8; ++i) {
        glm::vec3 n1 = getPosition(row + neighborOffsets[i][0], col + neighborOffsets[i][1]);
        glm::vec3 n2 = getPosition(row + neighborOffsets[(i + 1) % 8][0], col + neighborOffsets[(i + 1) % 8][1]);
        normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);
}

// glm::vec3 TerrainGenerator::getColor(float worldX, float worldZ) {
//     float scaledX = worldX / m_scale;
//     float scaledZ = worldZ / m_scale;
//     float normalizedHeight = getHeight(scaledX, scaledZ);

//     auto easeInOut = [](float t) {
//         return t * t * (3.0f - 2.0f * t);
//     };

//     float waterToSandStart = m_waterLevel;
//     float waterToSandEnd = m_waterLevel + m_waterTransition;
//     float sandToGrassStart = m_sandLevel;
//     float sandToGrassEnd = m_sandLevel + m_sandTransition;
//     float grassToMountainStart = m_grassLevel;
//     float grassToMountainEnd = m_grassLevel + m_grassTransition;

//     if (normalizedHeight <= waterToSandStart) {
//         return glm::vec3(0.0f, 0.0f, 0.0f);
//     }
//     else if (normalizedHeight <= waterToSandEnd) {
//         float t = (normalizedHeight - waterToSandStart) / m_waterTransition;
//         return glm::vec3(easeInOut(t), 0.0f, 0.0f);
//     }
//     else if (normalizedHeight <= sandToGrassStart) {
//         return glm::vec3(1.0f, 0.0f, 0.0f);
//     }
//     else if (normalizedHeight <= sandToGrassEnd) {
//         float t = (normalizedHeight - sandToGrassStart) / m_sandTransition;
//         float blend = easeInOut(t);
//         return glm::vec3(1.0f - blend, blend, 0.0f);
//     }
//     else if (normalizedHeight <= grassToMountainStart) {
//         return glm::vec3(0.0f, 1.0f, 0.0f);
//     }
//     else if (normalizedHeight <= grassToMountainEnd) {
//         float t = (normalizedHeight - grassToMountainStart) / m_grassTransition;
//         float blend = easeInOut(t);
//         return glm::vec3(0.0f, 1.0f - blend, blend);
//     }
//     else {
//         return glm::vec3(0.0f, 0.0f, 1.0f);
//     }
// }

glm::vec3 TerrainGenerator::getColor(float worldX, float worldZ){
    float scaledX = worldX / m_scale;
    float scaledZ = worldZ / m_scale;
    float normalizedHeight = getHeight(scaledX, scaledZ);
    if (normalizedHeight <= m_waterLevel) {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    } else if (normalizedHeight <= m_sandLevel) {
        return glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (normalizedHeight <= m_grassLevel) {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    } else if (normalizedHeight <= m_rockLevel) {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    } else {
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

float TerrainGenerator::computePerlin(float x, float y) {
    int x0 = std::floor(x);
    int y0 = std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    glm::vec2 v1(x - x0, y - y0);
    glm::vec2 v2(x - x1, y - y0);
    glm::vec2 v3(x - x0, y - y1);
    glm::vec2 v4(x - x1, y - y1);

    glm::vec2 g1 = sampleRandomVector(x0, y0);
    glm::vec2 g2 = sampleRandomVector(x1, y0);
    glm::vec2 g3 = sampleRandomVector(x0, y1);
    glm::vec2 g4 = sampleRandomVector(x1, y1);

    float d1 = glm::dot(g1, v1);
    float d2 = glm::dot(g2, v2);
    float d3 = glm::dot(g3, v3);
    float d4 = glm::dot(g4, v4);

    float dx = x - x0;
    float dy = y - y0;

    float ix1 = interpolate(d1, d2, dx);
    float ix2 = interpolate(d3, d4, dx);

    return interpolate(ix1, ix2, dy);
}

glm::vec2 TerrainGenerator::worldToLocal(float worldX, float worldZ) {
    return glm::vec2(
        fmod(worldX, CHUNK_SIZE) / CHUNK_SIZE,
        fmod(worldZ, CHUNK_SIZE) / CHUNK_SIZE
    );
}

glm::vec2 TerrainGenerator::localToWorld(float localX, float localZ, int chunkX, int chunkZ) {
    return glm::vec2(
        chunkX * CHUNK_SIZE + localX * CHUNK_SIZE,
        chunkZ * CHUNK_SIZE + localZ * CHUNK_SIZE
    );
}

float TerrainGenerator::getWorldHeight(float worldX, float worldZ) {
    float scaledX = worldX / m_scale;
    float scaledZ = worldZ / m_scale;
    float normalizedHeight = getHeight(scaledX, scaledZ);
    return mapHeight(normalizedHeight) * m_scale;
}

float TerrainGenerator::mapHeight(float normalizedHeight) {
    if (normalizedHeight <= m_waterLevel) {
        return 0;
    }
    float nHeight = (normalizedHeight - m_waterLevel) / (1.0f - m_waterLevel);
    return 1 - std::cos((nHeight * M_PI) / 2);
}

std::vector<float> TerrainGenerator::generateTerrainChunk(int chunkX, int chunkZ) {
    int verticesPerSide = static_cast<int>(CHUNK_SIZE / VERTEX_SPACING);
    size_t expectedSize = verticesPerSide * verticesPerSide * 6 * 11;
    std::vector<float> verts;
    verts.reserve(expectedSize);
    bool flipX = chunkX % 2 == 0;
    bool flipZ = chunkZ % 2 == 0;

    for (int x = 0; x < verticesPerSide; x++) {
        for (int z = 0; z < verticesPerSide; z++) {
            glm::vec2 worldPos = localToWorld(
                static_cast<float>(x) / verticesPerSide,
                static_cast<float>(z) / verticesPerSide,
                chunkX, chunkZ
            );

            glm::vec3 p1(worldPos.x, getWorldHeight(worldPos.x, worldPos.y), worldPos.y);
            glm::vec3 p2(worldPos.x + VERTEX_SPACING, getWorldHeight(worldPos.x + VERTEX_SPACING, worldPos.y), worldPos.y);
            glm::vec3 p3(worldPos.x, getWorldHeight(worldPos.x, worldPos.y + VERTEX_SPACING), worldPos.y + VERTEX_SPACING);
            glm::vec3 p4(worldPos.x + VERTEX_SPACING, getWorldHeight(worldPos.x + VERTEX_SPACING, worldPos.y + VERTEX_SPACING), worldPos.y + VERTEX_SPACING);

            // Calculate normals and colors
            glm::vec3 n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            glm::vec3 c1 = getColor(worldPos.x, worldPos.y);

            // Calculate UV coordinates
            glm::vec2 uv1(static_cast<float>(x) / verticesPerSide, static_cast<float>(z) / verticesPerSide);
            glm::vec2 uv2(static_cast<float>(x + 1) / verticesPerSide, static_cast<float>(z) / verticesPerSide);
            glm::vec2 uv3(static_cast<float>(x) / verticesPerSide, static_cast<float>(z + 1) / verticesPerSide);
            glm::vec2 uv4(static_cast<float>(x + 1) / verticesPerSide, static_cast<float>(z + 1) / verticesPerSide);

            if (flipX) {
                uv1 = glm::vec2(1.0f - uv1.x, uv1.y);
                uv2 = glm::vec2(1.0f - uv2.x, uv2.y);
                uv3 = glm::vec2(1.0f - uv3.x, uv3.y);
                uv4 = glm::vec2(1.0f - uv4.x, uv4.y);
            }

            if (flipZ) {
                uv1 = glm::vec2(uv1.x, 1.0f - uv1.y);
                uv2 = glm::vec2(uv2.x, 1.0f - uv2.y);
                uv3 = glm::vec2(uv3.x, 1.0f - uv3.y);
                uv4 = glm::vec2(uv4.x, 1.0f - uv4.y);
            }

            // Add first triangle
            addPointToVector(p1, n1, c1, uv1, verts);
            addPointToVector(p2, n1, c1, uv2, verts);
            addPointToVector(p3, n1, c1, uv3, verts);

            // Add second triangle
            addPointToVector(p2, n1, c1, uv2, verts);
            addPointToVector(p4, n1, c1, uv4, verts);
            addPointToVector(p3, n1, c1, uv3, verts);
        }
    }
    return verts;
}
