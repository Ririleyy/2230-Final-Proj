#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>
#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "utils/shapes/shapes.h"
#include "utils/shapes/cube.h"
#include "utils/shapes/cone.h"
#include "utils/shapes/sphere.h"
#include "utils/shapes/cylinder.h"
#include "utils/shapes/mesh.h"

struct LODKey
{
    PrimitiveType shapeType;
    int lodParam1;
    int lodParam2;
    bool operator==(const LODKey &other) const
    {
        return shapeType == other.shapeType && lodParam1 == other.lodParam1 && lodParam2 == other.lodParam2;
    }
};

namespace std
{
    template <>
    struct hash<LODKey>
    {
        size_t operator()(const LODKey &key) const
        {
            // Combine the hash of both members
            size_t h1 = hash<int>{}(static_cast<int>(key.shapeType));
            size_t h2 = hash<int>{}(key.lodParam1);
            size_t h3 = hash<int>{}(key.lodParam2);
            return h1 ^ (h2 << 1) ^ (h3 << 1);
        }
    };
}

class GeometryManager
{
public:
    GeometryManager();
    ~GeometryManager();
    const std::vector<float> &getUpdatedShapeParms(const PrimitiveType type);
    int getNumVertices(const RenderShapeData &shape, const glm::mat4 &getNumVertices);
    const std::vector<float> &getShapeData(const PrimitiveType type);    std::vector<float> &getMeshData(const std::string &filename);
    std::vector<float> &getLODShapeData(const LODKey &key);
    void clearMeshData();
    void clearLODShapeData();

    std::unordered_set<PrimitiveType> activatedShapes;
    std::unordered_map<PrimitiveType, std::unique_ptr<Shape>> canoShapeDict;
    std::unordered_map<LODKey, std::vector<float>> m_LODshapeData;
    LODKey generateLODKey(const RenderShapeData &shape, const glm::mat4 &invViewMatrix);
private:
    void createMeshData(const std::string &filename);
    void createLODShapeData(const LODKey &key);
    std::unordered_map<std::string, std::vector<float>> m_meshData;
    float min_distance = 0.1f;   
    float max_distance = 100.0f; 
    const float MIN_DISTANCE = 0.1f;   // Objects closer than this use max tessellation
    const float MAX_DISTANCE = 100.0f; // Objects farther than this use min tessellation
    const int MIN_TESS = 1;
    const int MAX_TESS = 50;
    const float FALLOFF_STEEPNESS = 4.0f;
    int calcLOD(float distance);

};
