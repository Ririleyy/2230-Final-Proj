#pragma once

#include <vector>
#include <glm/glm.hpp>

class Shape {
public:
    void updateParams(int param1, int param2);
    virtual ~Shape() = default;
    const virtual std::vector<float>& generateShape() { return m_vertexData; }
    int getNumVerticies() const { return m_vertexData.size() / 8; }

protected:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    virtual void setVertexData() = 0;
    virtual int getMinParam1() const = 0;  
    virtual int getMinParam2() const = 0;  
    std::vector<float> m_vertexData;
    float m_radius = 0.5f;
    const float rotateOffset = glm::radians(90.f);
    int m_param1;
    int m_param2;
};
