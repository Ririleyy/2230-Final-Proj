#include "utils/shapes/shapes.h"

void Shape::insertVec3(std::vector<float> &data, glm::vec3 v)
{
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Shape::insertVec2(std::vector<float> &data, glm::vec2 v)
{
    data.push_back(v.x);
    data.push_back(v.y);
}

void Shape::updateParams(int param1, int param2)
{
    m_vertexData = std::vector<float>();
    m_param1 = std::max(param1, getMinParam1());
    m_param2 = std::max(param2, getMinParam2());
    setVertexData();
}
