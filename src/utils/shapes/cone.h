#include "utils/shapes/shapes.h"

class Cone : public Shape
{
public:
    void setVertexData() override;

private:
    void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeTopTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight);
    void makeSideWedge(float currentTheta, float nextTheta);
    glm::vec2 getSideUV(glm::vec3 &pos);
    void makeTopWedge(float currentTheta, float nextTheta);
    glm::vec2 getTopUV(glm::vec3 &pos);
    float getRadiusAtHeight(float height);
    glm::vec3 getNormalAt(const glm::vec3 &point);
    int getMinParam1() const override { return 1; }
    int getMinParam2() const override { return 3; }
};