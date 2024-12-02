#include "utils/managers/geometrymanager.h"
#include <iostream>
#include "settings.h"
#include "utils/debug.h"
#include "utils/camera.h"
#include "settings.h"
#include <limits>

GeometryManager::GeometryManager()
{
    canoShapeDict[PrimitiveType::PRIMITIVE_CUBE] = std::make_unique<Cube>();
    canoShapeDict[PrimitiveType::PRIMITIVE_CONE] = std::make_unique<Cone>();
    canoShapeDict[PrimitiveType::PRIMITIVE_CYLINDER] = std::make_unique<Cylinder>();
    canoShapeDict[PrimitiveType::PRIMITIVE_SPHERE] = std::make_unique<Sphere>();
    canoShapeDict[PrimitiveType::PRIMITIVE_MESH] = std::make_unique<Mesh>();
}

GeometryManager::~GeometryManager()
{
    activatedShapes.clear();
    canoShapeDict.clear();
    m_meshData.clear();
    m_LODshapeData.clear();
}

const std::vector<float> &GeometryManager::getUpdatedShapeParms(const PrimitiveType type)
{
    canoShapeDict[type]->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    return canoShapeDict[type]->generateShape();
}

int GeometryManager::getNumVertices(const RenderShapeData &shape, const glm::mat4 &invViewMat)
{
    if (settings.extraCredit1)
    {
        LODKey key = generateLODKey(shape, invViewMat);
        return getLODShapeData(key).size() / 8;
    }
    return canoShapeDict[shape.primitive.type]->getNumVerticies();
}

void GeometryManager::createMeshData(const std::string &filename)
{
    canoShapeDict[PrimitiveType::PRIMITIVE_MESH]->readObj(filename);
    m_meshData[filename] = canoShapeDict[PrimitiveType::PRIMITIVE_MESH]->generateShape();
}

std::vector<float> &GeometryManager::getMeshData(const std::string &filename)
{
    if (m_meshData.find(filename) == m_meshData.end())
    {
        createMeshData(filename);
    }
    return m_meshData[filename];
}

void GeometryManager::clearMeshData()
{
    m_meshData.clear();
}

void GeometryManager::createLODShapeData(const LODKey &key)
{
    canoShapeDict[key.shapeType]->updateParams(key.lodParam1, key.lodParam2);
    m_LODshapeData[key] = canoShapeDict[key.shapeType]->generateShape();
}

void GeometryManager::clearLODShapeData()
{
    m_LODshapeData.clear();
}

int GeometryManager::calcLOD(float distance)
{
    float normalizedDist = (distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE);
    return std::round(normalizedDist * MAX_TESS);
}

LODKey GeometryManager::generateLODKey(const RenderShapeData &shape,const glm::mat4 &invViewMatrix)
{
    float dist = Camera::getObjectDistance(shape.ctm, invViewMatrix);
    int lod = calcLOD(dist);
    return {shape.primitive.type, settings.shapeParameter1 - lod, settings.shapeParameter2 - lod};
}

std::vector<float> &GeometryManager::getLODShapeData(const LODKey &key)
{
    if (m_LODshapeData.find(key) == m_LODshapeData.end())
    {
        createLODShapeData(key);
    }
    return m_LODshapeData[key];
}

const std::vector<float> &GeometryManager::getShapeData(const PrimitiveType type)
{
    return canoShapeDict[type]->generateShape();
}
