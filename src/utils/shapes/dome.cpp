// dome.cpp
#include "dome.h"

std::vector<float> Dome::generateDome(int param1, int param2) {
    std::vector<float> vertexData;
    param1 = std::max(param1, 2);  // minimum param1
    param2 = std::max(param2, 3);  // minimum param2

    // Step through phi (latitude) first
    float thetaStep = glm::radians(360.f / param2);
    for (int i = 0; i < param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(vertexData, currentTheta, nextTheta, param1);
    }

    return vertexData;
}

void Dome::makeWedge(std::vector<float>& vertexData, float currentTheta, float nextTheta, int param1) {
    // Then step through theta (longitude)
    float phiStep = glm::radians(90.f / param1);
    for (int i = 0; i < param1; i++) {
        glm::vec3 topLeft = getSphericalCoord(nextTheta, i * phiStep);
        glm::vec3 topRight = getSphericalCoord(currentTheta, i * phiStep);
        glm::vec3 bottomLeft = getSphericalCoord(nextTheta, (i + 1) * phiStep);
        glm::vec3 bottomRight = getSphericalCoord(currentTheta, (i + 1) * phiStep);
        makeTile(vertexData, topLeft, topRight, bottomLeft, bottomRight);
    }
}

glm::vec2 Dome::getUV(glm::vec3 &pos) {
    float u, v;
    v = std::asin(pos.y / 0.5f) / M_PI + 0.5f;
    float theta = std::atan2(pos.z, pos.x);
    theta = (theta < 0) ? theta + 2 * M_PI : theta;
    u = 1 - theta / (2 * M_PI);
    return glm::vec2(u, v);
}

glm::vec3 Dome::getSphericalCoord(const float &theta, const float &phi, const float radius) {
    return glm::vec3(radius * cos(theta) * sin(phi),
                     radius * cos(phi),
                     radius * sin(theta) * sin(phi));
}

void Dome::makeTile(std::vector<float>& vertexData,
                    glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 topLeftNormal1 = -glm::normalize(topLeft);
    glm::vec2 topLeftUV = getUV(topLeft);
    glm::vec3 bottomRightNormal1 = -glm::normalize(bottomRight);
    glm::vec2 bottomRightUV = getUV(bottomRight);
    glm::vec3 topRightNormal = -glm::normalize(topRight);
    glm::vec2 topRightUV = getUV(topRight);

    glm::vec3 topLeftNormal2 = -glm::normalize(topLeft);
    glm::vec3 bottomLeftNormal = -glm::normalize(bottomLeft);
    glm::vec2 bottomLeftUV = getUV(bottomLeft);
    glm::vec3 bottomRightNormal2 = -glm::normalize(bottomRight);

    insertVec3(vertexData, topLeft);
    insertVec3(vertexData, topLeftNormal1);
    insertVec2(vertexData, topLeftUV);
    insertVec3(vertexData, bottomRight);
    insertVec3(vertexData, bottomRightNormal1);
    insertVec2(vertexData, bottomRightUV);
    insertVec3(vertexData, topRight);
    insertVec3(vertexData, topRightNormal);
    insertVec2(vertexData, topRightUV);
    insertVec3(vertexData, topLeft);
    insertVec3(vertexData, topLeftNormal2);
    insertVec2(vertexData, topLeftUV);
    insertVec3(vertexData, bottomLeft);
    insertVec3(vertexData, bottomLeftNormal);
    insertVec2(vertexData, bottomLeftUV);
    insertVec3(vertexData, bottomRight);
    insertVec3(vertexData, bottomRightNormal2);
    insertVec2(vertexData, bottomRightUV);
}

void Dome::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Dome::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}