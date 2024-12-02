#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();
    traverseTree(root, renderData);


    return true;
}

void SceneParser::traverseTree(SceneNode* node, RenderData& renderData, const glm::mat4 parentCTM) {
    if (node == nullptr) return;
    glm::mat4 ctm = glm::mat4(1.0f);
    for (SceneTransformation* transformation : node->transformations) {
        switch (transformation->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = glm::translate(ctm, transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            ctm = glm::rotate(ctm, transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = glm::scale(ctm, transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = transformation->matrix;
            break;
        }
    }
    ctm = parentCTM * ctm;
    for (ScenePrimitive* primitive : node->primitives) {
        RenderShapeData shape;
        shape.primitive = *primitive;
        shape.ctm = ctm;
        renderData.shapes.push_back(shape);
    }
    for (SceneLight* light : node->lights) {
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light->color;
        lightData.function = light->function;
        if (light->type != LightType::LIGHT_DIRECTIONAL) {
            lightData.pos = ctm * glm::vec4(0, 0, 0, 1);
        }
        if (light->type != LightType::LIGHT_POINT) {
            lightData.dir = ctm * light->dir;
        }
        if (light->type == LightType::LIGHT_SPOT) {
            lightData.penumbra = light->penumbra;
            lightData.angle = light->angle;
        }
        lightData.width = light->width;
        lightData.height = light->height;
        renderData.lights.push_back(lightData);
    }
    for (SceneNode* child : node->children) {
        traverseTree(child, renderData, ctm);
    }
}
