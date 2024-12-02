#include "utils/shapes/cube.h"
#include <iostream>

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight)
{
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 topLeftNormal1 = glm::normalize(glm::cross(bottomRight - topLeft, topRight - topLeft));
    glm::vec3 bottomRightNormal1 = glm::normalize(glm::cross(topRight - bottomRight, topLeft - bottomRight));
    glm::vec3 topRightNormal = glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight));

    glm::vec3 topLeftNormal2 = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft));
    glm::vec3 bottomLeftNormal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));
    glm::vec3 bottomRightNormal2 = glm::normalize(glm::cross(topLeft - bottomRight, bottomLeft - bottomRight));

    glm::vec2 topLeftUV = getUV(topLeft);
    glm::vec2 topRightUV = getUV(topRight);
    glm::vec2 bottomLeftUV = getUV(bottomLeft);
    glm::vec2 bottomRightUV = getUV(bottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, topLeftNormal1);
    insertVec2(m_vertexData, topLeftUV);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bottomRightNormal1);
    insertVec2(m_vertexData, bottomRightUV);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, topRightNormal);
    insertVec2(m_vertexData, topRightUV);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, topLeftNormal2);
    insertVec2(m_vertexData, topLeftUV);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, bottomLeftNormal);
    insertVec2(m_vertexData, bottomLeftUV);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bottomRightNormal2);
    insertVec2(m_vertexData, bottomRightUV);
}
glm::vec2 Cube::getUV(glm::vec3 &pos)
{
    float u = 0.0f, v = 0.0f;
    if (pos.z == 0.5f)
    {
        u = 0.5f + pos.x;
        v = 0.5f + pos.y;
    }
    else if (pos.z == -0.5f)
    {
        u = 0.5f - pos.x;
        v = 0.5f + pos.y;
    }
    else if (pos.x == 0.5f)
    {
        u = 0.5f - pos.z;
        v = 0.5f + pos.y;
    }
    else if (pos.x == -0.5f)
    {
        u = 0.5f + pos.z;
        v = 0.5f + pos.y;
    }
    else if (pos.y == 0.5f)
    {
        u = 0.5f + pos.x;
        v = 0.5f - pos.z;
    }
    else if (pos.y == -0.5f)
    {
        u = 0.5f + pos.x;
        v = 0.5f + pos.z;
    } else {
        std::cerr << "Invalid position" << std::endl;
    }
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);
    return glm::vec2(u, v);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight)
{
    float singleFaceLength = 1.0f / m_param1;
    for (int i = 0; i < m_param1; i++)
    {
        for (int j = 0; j < m_param1; j++)
        {
            glm::vec3 topLeftTile = topLeft + i * singleFaceLength * (topRight - topLeft) + j * singleFaceLength * (bottomLeft - topLeft);
            glm::vec3 topRightTile = topLeft + (i + 1) * singleFaceLength * (topRight - topLeft) + j * singleFaceLength * (bottomLeft - topLeft);
            glm::vec3 bottomLeftTile = topLeft + i * singleFaceLength * (topRight - topLeft) + (j + 1) * singleFaceLength * (bottomLeft - topLeft);
            glm::vec3 bottomRightTile = topLeft + (i + 1) * singleFaceLength * (topRight - topLeft) + (j + 1) * singleFaceLength * (bottomLeft - topLeft);
            makeTile(topLeftTile, topRightTile, bottomLeftTile, bottomRightTile);
        }
    }
}

void Cube::setVertexData()
{
    // front face
    makeFace(glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f));

    // right face
    makeFace(
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f));
    // back face
    makeFace(
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f));
    // left face
    makeFace(
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f));
    // top face
    makeFace(
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    // bottom face
    makeFace(
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f));
}
