#include "terrain.h"

#include <cmath>
#include "glm/glm.hpp"
#include <iostream>

// Constructor
TerrainGenerator::TerrainGenerator()
{
    // Task 8: turn off wireframe shading
    //m_wireshade = true; // STENCIL CODE
    m_wireshade = false; // TA SOLUTION

    // Define resolution of terrain generation
    m_resolution = 100;

    // Generate random vector lookup table
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
        m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

// Destructor
TerrainGenerator::~TerrainGenerator()
{
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

    vector.push_back(uv.x); // Add U coordinate
    vector.push_back(uv.y); // Add V coordinate

}
glm::vec2 calculateUV(int x, int y, int resolution) {
    // Normalize the UV coordinates to the [0, 1] range

    float u = static_cast<float>(x) / static_cast<float>(resolution - 1);
    float v = static_cast<float>(y) / static_cast<float>(resolution - 1);

    // Clamp the values to [0, 1] to avoid exceeding the range
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);
    //std::cout << "UV for vertex (" << x << ", " << y << "): (" << u << ", " << v << ")" << std::endl;
    return glm::vec2(u, v);
}
// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for (int x = 0; x < m_resolution; x++) {
        for (int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1, y1);
            glm::vec3 p2 = getPosition(x2, y1);
            glm::vec3 p3 = getPosition(x2, y2);
            glm::vec3 p4 = getPosition(x1, y2);

            glm::vec3 n1 = getNormal(x1, y1);
            glm::vec3 n2 = getNormal(x2, y1);
            glm::vec3 n3 = getNormal(x2, y2);
            glm::vec3 n4 = getNormal(x1, y2);

            glm::vec3 c1 = getColor(n1, p1);
            glm::vec3 c2 = getColor(n2, p2);
            glm::vec3 c3 = getColor(n3, p3);
            glm::vec3 c4 = getColor(n4, p4);

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


// Samples the (infinite) random vector grid at (row, col)
glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.
    float x = 1.0 * row / m_resolution;
    float y = 1.0 * col / m_resolution;
    float z = getHeight(x, y);
    return glm::vec3(x,y,z);
}

// ================== Students, please focus on the code below this point

// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    // alpha ease funcion
    float alpha_ease = 3 * alpha * alpha - 2 * alpha * alpha * alpha;

    float result = A + alpha_ease * (B - A);

    return result;
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {


    float total = 0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxAmplitude = 0;  // Used to normalize the result to the range [0, 1]

    for (int i = 0; i < 5; i++) {
        total += computePerlin(x * frequency, y * frequency) * amplitude;


        maxAmplitude += amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }


    return total / maxAmplitude;



}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    // Task 9: Compute the average normal for the given input indices
    // TA SOLUTION
    glm::vec3 normal = glm::vec3(0, 0, 0);
    std::vector<std::vector<int>> neighborOffsets = { // Counter-clockwise around the vertex
        {-1, -1},
        { 0, -1},
        { 1, -1},
        { 1,  0},
        { 1,  1},
        { 0,  1},
        {-1,  1},
        {-1,  0}
    };
    glm::vec3 V = getPosition(row,col);
    for (int i = 0; i < 8; ++i) {
        int n1RowOffset = neighborOffsets[i][0];
        int n1ColOffset = neighborOffsets[i][1];
        int n2RowOffset = neighborOffsets[(i + 1) % 8][0];
        int n2ColOffset = neighborOffsets[(i + 1) % 8][1];
        glm::vec3 n1 = getPosition(row + n1RowOffset, col + n1ColOffset);
        glm::vec3 n2 = getPosition(row + n2RowOffset, col + n2ColOffset);
        normal = normal + glm::cross(n1 - V, n2 - V);
    }
    return glm::normalize(normal);

}

// Computes color of vertex using normal and, optionally, position
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position
    // bool height_z =false;
    // if (position.z > 0.0f){

    //     height_z = true;

    // }

    // //normal
    // glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    // float dotProduct = glm::dot(normal, up);
    // bool normal_z = false;
    // if (dotProduct > 0.8f){

    //     normal_z = true;


    // }

    // if (height_z && normal_z) {
    //     return glm::vec3(1.0f, 1.0f, 1.0f);
    // } else {
    //     return glm::vec3(0.5f, 0.5f, 0.5f);
    // }
    return glm::vec3(0.5f, 0.5f, 0.5f);


}

// Computes the intensity of Perlin noise at some point
float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int x_round = std::floor(x);
    int y_round = std::floor(y);
    //4 neighbours (x_round,y_round),(x_round + 1,y_round),(x_round,y_round + 1), (x_round + 1,y_round + 1)
    float x1 = x_round,y1 = y_round;
    float x2 = x_round + 1,y2 = y_round;
    float x3 = x_round,y3 = y_round + 1;
    float x4 = x_round + 1,y4 = y_round + 1;


    // Task 2: compute offset vectors
    glm::vec2 v1 = {x - x1,y - y1};
    glm::vec2 v2 = {x - x2,y - y2};
    glm::vec2 v3 = {x - x3,y - y3};
    glm::vec2 v4 = {x - x4,y - y4};

    glm::vec2 d1 = sampleRandomVector(x1, y1);
    glm::vec2 d2 = sampleRandomVector(x2, y2);
    glm::vec2 d3 = sampleRandomVector(x3, y3);
    glm::vec2 d4 = sampleRandomVector(x4, y4);
    // Task 3: compute the dot product between the grid point direction vectors and its offset vectors
    float A = glm::dot(d1,v1); // dot product between top-left direction and its offset
    float B = glm::dot(d2,v2); // dot product between top-right direction and its offset
    float C = glm::dot(d3,v3); // dot product between bottom-right direction and its offset
    float D = glm::dot(d4,v4); // dot product between bottom-left direction and its offset

    // Task 5: Debug this line to properly use your interpolation function to produce the correct value
    float dx = x - x1;
    float dy = y - y1;

    float G = interpolate(A, B, dx);
    float H = interpolate(C, D, dx);

    float result = interpolate(G, H, dy);
    return result;


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
    // Scale down the coordinates for Perlin noise
    float scaledX = worldX * 0.02f;
    float scaledZ = worldZ * 0.02f;
    return getHeight(scaledX, scaledZ) * 100.0f; // Amplify the height
}

std::vector<float> TerrainGenerator::generateTerrainChunk(int chunkX, int chunkZ) {
    std::vector<float> verts;
    int verticesPerSide = static_cast<int>(CHUNK_SIZE / VERTEX_SPACING);
    
    for (int x = 0; x < verticesPerSide; x++) {
        for (int z = 0; z < verticesPerSide; z++) {
            // Calculate world positions
            glm::vec2 worldPos = localToWorld(
                static_cast<float>(x) / verticesPerSide,
                static_cast<float>(z) / verticesPerSide,
                chunkX, chunkZ
            );
            
            // Generate vertices for two triangles
            glm::vec3 p1(worldPos.x, getWorldHeight(worldPos.x, worldPos.y), worldPos.y);
            glm::vec3 p2(worldPos.x + VERTEX_SPACING, getWorldHeight(worldPos.x + VERTEX_SPACING, worldPos.y), worldPos.y);
            glm::vec3 p3(worldPos.x, getWorldHeight(worldPos.x, worldPos.y + VERTEX_SPACING), worldPos.y + VERTEX_SPACING);
            glm::vec3 p4(worldPos.x + VERTEX_SPACING, getWorldHeight(worldPos.x + VERTEX_SPACING, worldPos.y + VERTEX_SPACING), worldPos.y + VERTEX_SPACING);
            
            // Calculate normals and colors
            glm::vec3 n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            glm::vec3 color = getColor(n1, p1);
            
            // Calculate UV coordinates
            glm::vec2 uv1(static_cast<float>(x) / verticesPerSide, static_cast<float>(z) / verticesPerSide);
            glm::vec2 uv2(static_cast<float>(x + 1) / verticesPerSide, static_cast<float>(z) / verticesPerSide);
            glm::vec2 uv3(static_cast<float>(x) / verticesPerSide, static_cast<float>(z + 1) / verticesPerSide);
            glm::vec2 uv4(static_cast<float>(x + 1) / verticesPerSide, static_cast<float>(z + 1) / verticesPerSide);
            
            // Add first triangle
            addPointToVector(p1, n1, color, uv1, verts);
            addPointToVector(p2, n1, color, uv2, verts);
            addPointToVector(p3, n1, color, uv3, verts);
            
            // Add second triangle
            addPointToVector(p2, n1, color, uv2, verts);
            addPointToVector(p4, n1, color, uv4, verts);
            addPointToVector(p3, n1, color, uv3, verts);
        }
    }
    
    return verts;
}
