#include "utils/shapes/sphere.h"

void Sphere::setVertexData()
{
    // Step through phi (latitude) first
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++)
    {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::makeWedge(float currentTheta, float nextTheta)
{
    // Then step through theta (longitude)
    float phiStep = glm::radians(180.f / m_param1);
    for (int i = 0; i < m_param1; i++)
    {
        glm::vec3 topLeft = getSphericalCoord(nextTheta, i * phiStep);
        glm::vec3 topRight = getSphericalCoord(currentTheta, i * phiStep);
        glm::vec3 bottomLeft = getSphericalCoord(nextTheta, (i + 1) * phiStep);
        glm::vec3 bottomRight = getSphericalCoord(currentTheta, (i + 1) * phiStep);
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

glm::vec2 Sphere::getUV(glm::vec3 &pos)
{
    float u, v;
    v = std::asin(pos.y / 0.5f) / M_PI + 0.5f;
    float theta = std::atan2(pos.z, pos.x);
    theta = (theta < 0) ? theta + 2 * M_PI : theta;
    u = 1 - theta / (2 * M_PI);
    return glm::vec2(u, v);
}

glm::vec3 Sphere::getSphericalCoord(const float &theta, const float &phi, const float radius)
{
    return glm::vec3(radius * cos(theta) * sin(phi),
                     radius * cos(phi),
                     radius * sin(theta) * sin(phi));
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight)
{
    glm::vec3 topLeftNormal1 = glm::normalize(topLeft);
    glm::vec2 topLeftUV = getUV(topLeft);
    glm::vec3 bottomRightNormal1 = glm::normalize(bottomRight);
    glm::vec2 bottomRightUV = getUV(bottomRight);
    glm::vec3 topRightNormal = glm::normalize(topRight);
    glm::vec2 topRightUV = getUV(topRight);

    glm::vec3 topLeftNormal2 = glm::normalize(topLeft);
    glm::vec3 bottomLeftNormal = glm::normalize(bottomLeft);
    glm::vec2 bottomLeftUV = getUV(bottomLeft);
    glm::vec3 bottomRightNormal2 = glm::normalize(bottomRight);

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
