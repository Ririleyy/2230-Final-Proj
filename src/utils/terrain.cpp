#include "terrain.h"

#include <cmath>
#include "glm/glm.hpp"

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

// Helper for generateTerrain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.y);
    vector.push_back(point.z);
}

// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for(int x = 0; x < m_resolution; x++) {
        for(int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1,y1);
            glm::vec3 p2 = getPosition(x2,y1);
            glm::vec3 p3 = getPosition(x2,y2);
            glm::vec3 p4 = getPosition(x1,y2);

            glm::vec3 n1 = getNormal(x1,y1);
            glm::vec3 n2 = getNormal(x2,y1);
            glm::vec3 n3 = getNormal(x2,y2);
            glm::vec3 n4 = getNormal(x1,y2);

            // tris 1
            // x1y1z1
            // x2y1z2
            // x2y2z3
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p2, verts);
            addPointToVector(n2, verts);
            addPointToVector(getColor(n2, p2), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            // tris 2
            // x1y1z1
            // x2y2z3
            // x1y2z4
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p4, verts);
            addPointToVector(n4, verts);
            addPointToVector(getColor(n4, p4), verts);
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

