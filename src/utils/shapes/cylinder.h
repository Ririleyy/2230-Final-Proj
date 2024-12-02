#include "utils/shapes/shapes.h"

class Cylinder : public Shape {
public:
    void setVertexData() override;

private:
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    glm::vec2 getSideUV(glm::vec3 &pos);
    void makeTopTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    glm::vec2 getTopUV(glm::vec3 &pos);
    void makeSideWedge(float currentTheta, float nextTheta);
    void makeTopWedge(float currentTheta, float nextTheta, bool isTop);
    int getMinParam1() const override { return 1; }
    int getMinParam2() const override { return 3; }
};
