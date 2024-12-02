#pragma once
#include "utils/shapes/shapes.h"

class Cube : public Shape {
public:
    void setVertexData() override;

private:
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    glm::vec2 getUV(glm::vec3 &pos);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    int getMinParam1() const override { return 1; }
    int getMinParam2() const override { return 1; }
};