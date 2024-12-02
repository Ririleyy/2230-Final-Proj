#pragma once
#include "utils/shapes/shapes.h"

class Sphere : public Shape {
public:
    void setVertexData() override;

private:
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    glm::vec3 getSphericalCoord(const float &theta, const float &phi, const float radius=0.5f);
    int getMinParam1() const override { return 2; }
    int getMinParam2() const override { return 3; }
    glm::vec2 getUV(glm::vec3 &pos);
};