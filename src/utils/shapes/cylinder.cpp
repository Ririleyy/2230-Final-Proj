#include "utils/shapes/cylinder.h"

void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight)
{
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    glm::vec3 topCenter = glm::vec3(0.f, topLeft.y, 0.f);
    glm::vec3 bottomCenter = glm::vec3(0.f, bottomLeft.y, 0.f);

    glm::vec3 topLeftNormal1 = glm::normalize(topLeft - topCenter);
    glm::vec2 topLeftUV = getSideUV(topLeft);
    glm::vec3 bottomRightNormal1 = glm::normalize(bottomRight - bottomCenter);
    glm::vec2 bottomRightUV = getSideUV(bottomRight);
    glm::vec3 topRightNormal = glm::normalize(topRight - topCenter);
    glm::vec2 topRightUV = getSideUV(topRight);

    glm::vec3 topLeftNormal2 = glm::normalize(topLeft - topCenter);
    glm::vec3 bottomLeftNormal = glm::normalize(bottomLeft - bottomCenter);
    glm::vec2 bottomLeftUV = getSideUV(bottomLeft);
    glm::vec3 bottomRightNormal2 = glm::normalize(bottomRight - bottomCenter);

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

void Cylinder::makeTopTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight)
{
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    glm::vec3 normal = glm::normalize(glm::vec3(0.f, topLeft.y, 0.f));
    glm::vec2 topLeftUV = getTopUV(topLeft);
    glm::vec2 topRightUV = getTopUV(topRight);
    glm::vec2 bottomLeftUV = getTopUV(bottomLeft);
    glm::vec2 bottomRightUV = getTopUV(bottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, topLeftUV);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, bottomRightUV);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, topRightUV);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, topLeftUV);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, bottomLeftUV);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, bottomRightUV);
}

void Cylinder::makeSideWedge(float currentTheta, float nextTheta)
{
    float heightStep = 1.f / m_param1;
    for (int i = 0; i < m_param1; i++)
    {
        float currentHeight = 0.5f - i * heightStep;
        float nextHeight = 0.5f - (i + 1) * heightStep;
        glm::vec3 topLeft = glm::vec3(m_radius * cos(nextTheta), currentHeight, m_radius * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(m_radius * cos(currentTheta), currentHeight, m_radius * sin(currentTheta));
        glm::vec3 bottomLeft = glm::vec3(m_radius * cos(nextTheta), nextHeight, m_radius * sin(nextTheta));
        glm::vec3 bottomRight = glm::vec3(m_radius * cos(currentTheta), nextHeight, m_radius * sin(currentTheta));
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

glm::vec2 Cylinder::getSideUV(glm::vec3 &pos)
{
  float u, v;
  v = pos.y + 0.5f;
  float theta = std::atan2(pos.z, pos.x);
  theta = (theta < 0) ? theta + 2 * M_PI : theta;
  u = 1 - theta / (2 * M_PI);
  return glm::vec2(u, v);
}

void Cylinder::makeTopWedge(float currentTheta, float nextTheta, bool isTop)
{
    float yPosition = isTop ? 0.5f : -0.5f;
    if (!isTop)
    {
        std::swap(currentTheta, nextTheta);
    }
    for (int i = 0; i < m_param1; i++)
    {
        float currentRadius = i * (m_radius / m_param1);
        float nextradius = (i + 1) * (m_radius / m_param1);
        glm::vec3 topLeft = glm::vec3(currentRadius * cos(nextTheta), yPosition, currentRadius * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(currentRadius * cos(currentTheta), yPosition, currentRadius * sin(currentTheta));
        glm::vec3 bottomLeft = glm::vec3(nextradius * cos(nextTheta), yPosition, nextradius * sin(nextTheta));
        glm::vec3 bottomRight = glm::vec3(nextradius * cos(currentTheta), yPosition, nextradius * sin(currentTheta));
        makeTopTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

glm::vec2 Cylinder::getTopUV(glm::vec3 &pos)
{
  float u = 0.5f + pos.x;
  float v = (pos.y > 0) ? 0.5f - pos.z : 0.5f + pos.z;
  return glm::vec2(u, v);
}


void Cylinder::setVertexData()
{
    float thetStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++)
    {
        float currentTheta = i * thetStep;
        float nextTheta = (i + 1) * thetStep;
        makeSideWedge(currentTheta, nextTheta);
        makeTopWedge(currentTheta, nextTheta, true);
        makeTopWedge(currentTheta, nextTheta, false);
    }
}
