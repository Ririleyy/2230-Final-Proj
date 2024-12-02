#include "utils/shapes/cone.h"

glm::vec3 Cone::getNormalAt(const glm::vec3 &point)
{
    float nX = 2.0f * point.x;
    float nY = -0.5f * point.y + 0.25f;
    float nZ = 2.0f * point.z;
    return glm::normalize(glm::vec3(nX, nY, nZ));
}

float Cone::getRadiusAtHeight(float height)
{
    return m_radius / 2.f - m_radius * height;
}

glm::vec2 Cone::getSideUV(glm::vec3 &pos)
{
    float u, v;
    v = pos.y + 0.5f;
    float theta = std::atan2(pos.z, pos.x);
    if (theta >= 0)
    {
        u = 1 - theta / (2 * M_PI);
    }
    else
    {
        u = -theta / (2 * M_PI);
    }
    return glm::vec2(u, v);
}

void Cone::makeSideTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight)
{
    glm::vec3 topLeftNormal1;
    glm::vec3 topRightNormal;
    glm::vec3 topLeftNormal2;
    glm::vec3 bottomRightNormal1 = getNormalAt(bottomRight);
    glm::vec3 bottomLeftNormal = getNormalAt(bottomLeft);
    glm::vec3 bottomRightNormal2 = getNormalAt(bottomRight);
    if (std::abs(topLeft.y - 0.5f) < 0.001f)
    {
        float avgX = (bottomLeftNormal.x + bottomRightNormal1.x) / 2.f;
        float avgZ = (bottomLeftNormal.z + bottomRightNormal1.z) / 2.f;
        float y = bottomLeftNormal.y;
        glm::vec3 averageNormal = glm::normalize(glm::vec3(avgX, y, avgZ));
        // glm::vec3 averageNormal = glm::normalize(bottomLeftNormal + bottomRightNormal1);
        topLeftNormal1 = averageNormal;
        topRightNormal = averageNormal;
        topLeftNormal2 = averageNormal;
    }
    else
    {

        topLeftNormal1 = getNormalAt(topLeft);
        topRightNormal = getNormalAt(topRight);
        topLeftNormal2 = getNormalAt(topLeft);
    }

    glm::vec2 topLeftUV = getSideUV(topLeft);
    glm::vec2 topRightUV = getSideUV(topRight);
    glm::vec2 bottomLeftUV = getSideUV(bottomLeft);
    glm::vec2 bottomRightUV = getSideUV(bottomRight);

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

glm::vec2 Cone::getTopUV(glm::vec3 &pos)
{
    float u = 0.5f + pos.x;
    float v = 0.5f - pos.z;
    return glm::vec2(u, v);
}

void Cone::makeTopTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight)
{
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

void Cone::makeSideWedge(float currentTheta, float nextTheta)
{
    float heightStep = 1.f / m_param1;
    for (int i = 0; i < m_param1; i++)
    {
        float currentHeight = 0.5f - i * heightStep;
        float nextHeight = 0.5f - (i + 1) * heightStep;
        float currentRadius = getRadiusAtHeight(currentHeight);
        float nextRadius = getRadiusAtHeight(nextHeight);

        glm::vec3 topLeft = glm::vec3(currentRadius * cos(nextTheta), currentHeight, currentRadius * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(currentRadius * cos(currentTheta), currentHeight, currentRadius * sin(currentTheta));
        glm::vec3 bottomLeft = glm::vec3(nextRadius * cos(nextTheta), nextHeight, nextRadius * sin(nextTheta));
        glm::vec3 bottomRight = glm::vec3(nextRadius * cos(currentTheta), nextHeight, nextRadius * sin(currentTheta));
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Cone::makeTopWedge(float currentTheta, float nextTheta)
{
    float yPosition = -0.5f;
    std::swap(currentTheta, nextTheta);
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

void Cone::setVertexData()
{
    float thetStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++)
    {
        float currentTheta = i * thetStep;
        float nextTheta = (i + 1) * thetStep;
        makeSideWedge(currentTheta, nextTheta);
        makeTopWedge(currentTheta, nextTheta);
    }
}
