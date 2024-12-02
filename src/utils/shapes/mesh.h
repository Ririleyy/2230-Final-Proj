#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "shapes.h"

struct vertData
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
};

class Mesh : public Shape
{
public:
    Mesh() {m_vertexData.clear(); };
    void setVertexData() override;
    void readObj(const std::string &filename) override;
private:
    std::string m_filename;
    vertData m_vd;
    void makeTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    void makeTriangle(glm::vec3 v0, glm::vec3 n0, glm::vec3 v1, glm::vec3 n1, glm::vec3 v2, glm::vec3 n2);
    std::vector<int> processFaceField(std::string field);
    unsigned int m_numIndices;
    int getMinParam1() const override { return 1; }
    int getMinParam2() const override { return 1; }
};
