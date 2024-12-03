// dome.h
#pragma once
#include <vector>
#include <glm/glm.hpp>

class Dome {
public:
    static std::vector<float> generateDome(int param1, int param2);

private:
    static void makeWedge(std::vector<float>& vertexData, float currTheta, float nextTheta, int param1);
    static void makeTile(std::vector<float>& vertexData,
                        glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight);
    static glm::vec3 getSphericalCoord(const float &theta, const float &phi, const float radius=0.5f);
    static glm::vec2 getUV(glm::vec3 &pos);
    static void insertVec3(std::vector<float> &data, glm::vec3 v);
    static void insertVec2(std::vector<float> &data, glm::vec2 v);
};