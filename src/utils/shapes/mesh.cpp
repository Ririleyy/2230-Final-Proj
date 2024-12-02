#include "mesh.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cassert>
#include "utils/debug.h"

void Mesh::setVertexData()
{
    std::ifstream file(m_filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open " << m_filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        try
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix.empty() || prefix[0] == '#')
            {
                continue;
            }

            if (prefix == "v" || prefix == "vn")    // ignore uv for now
            {
                glm::vec3 v;
                if (!(iss >> v.x >> v.y >> v.z))
                {
                    std::cerr << "Cannot parse vector data" << std::endl;
                    return;
                }

                if (prefix == "v")
                        m_vd.vertices.push_back(v);
                else if (prefix == "vn")
                    m_vd.normals.push_back(glm::normalize(v));

            }
            else if (prefix == "f")
            {
                std::string fields;
                std::vector<int> vertexIndices;
                std::vector<int> normalIndices;

                // Count vertices in face
                int vertexCount = 0;
                while (iss >> fields)
                {
                    vertexCount++;
                }

                if (vertexCount != 3)
                {
                    std::cerr << "Non-triangular face detected" << std::endl;
                    return;
                }

                // Reset stream to process vertices
                iss.clear();
                iss.seekg(0);
                iss >> prefix; // Skip the 'f' prefix

                while (iss >> fields)
                {
                    std::vector<int> indices = processFaceField(fields);    // for each vertex, determine its pos/normal
                    if (indices.empty())
                        continue;

                    // Handle negative indices
                    int vertexIndex = indices[0];
                    if (vertexIndex < 0)
                        vertexIndex = m_vd.vertices.size() + vertexIndex;
                    else
                        vertexIndex--;

                    if (vertexIndex >= m_vd.vertices.size())
                    {
                        std::cerr << "Vertex index out of bounds" << std::endl;
                        return;
                    }
                    vertexIndices.push_back(vertexIndex);

                    if (indices.size() >= 2)
                    {
                        int normalIndex = (indices.size() == 2) ? indices[1] : indices[2];
                        if (normalIndex < 0)
                            normalIndex = m_vd.normals.size() + normalIndex;
                        else
                            normalIndex--;

                        if (normalIndex >= m_vd.normals.size())
                        {
                            std::cerr << "Normal index out of bounds" << std::endl;
                            return;
                        }
                        normalIndices.push_back(normalIndex);
                    }
                }

                if (vertexIndices.size() != 3)
                {
                    std::cerr << "Invalid face definition" << std::endl;
                    return;
                }

                if (normalIndices.empty())
                {
                    makeTriangle(
                        m_vd.vertices[vertexIndices[0]],
                        m_vd.vertices[vertexIndices[1]],
                        m_vd.vertices[vertexIndices[2]]);
                }
                else
                {
                    makeTriangle(
                        m_vd.vertices[vertexIndices[0]], m_vd.normals[normalIndices[0]],
                        m_vd.vertices[vertexIndices[1]], m_vd.normals[normalIndices[1]],
                        m_vd.vertices[vertexIndices[2]], m_vd.normals[normalIndices[2]]);
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing line: " << line << std::endl;
            std::cerr << "Error: " << e.what() << std::endl;
            return;
        }
    }
    return;
}

void Mesh::readObj(const std::string &filename)
{
    m_filename = filename;
    m_vd.vertices.clear();
    m_vd.normals.clear();
    m_vd.uvs.clear();
    m_vertexData.clear();
    setVertexData();
}

void Mesh::makeTriangle(glm::vec3 v0, glm::vec3 n0, glm::vec3 v1, glm::vec3 n1, glm::vec3 v2, glm::vec3 n2)
{
    insertVec3(m_vertexData, v0);
    insertVec3(m_vertexData, n0);
    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, n1);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, n2);
}

void Mesh::makeTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    glm::vec3 n0 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    glm::vec3 n1 = glm::normalize(glm::cross(v2 - v1, v0 - v1));
    glm::vec3 n2 = glm::normalize(glm::cross(v0 - v2, v1 - v2));
    makeTriangle(v0, n0, v1, n1, v2, n2);
}

std::vector<int> Mesh::processFaceField(std::string field)
{
    std::vector<int> indices;
    std::stringstream ss(field);
    while (std::getline(ss, field, '/'))
    {
        if (!field.empty())
        {
            indices.push_back(std::stoi(field));
        }
    }
    return indices;
}
