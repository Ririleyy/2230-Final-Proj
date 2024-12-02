#include "utils/managers/uniformmanager.h"

#include <cassert>
#include <iostream>
#include "utils/camera.h"
#include "settings.h"
#include "utils/debug.h"

GLuint UniformManager::getUniformLocation(const char *name)
{
    assert(m_shader != 0);
    return glGetUniformLocation(m_shader, name);
}

void UniformManager::setSceneGlobalUniform(SceneGlobalData &globalData)
{
    setFloat("k_a", globalData.ka);
    setFloat("k_d", globalData.kd);
    setFloat("k_s", globalData.ks);
}

void UniformManager::setMVPUniform(const glm::mat4 &projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4 &ctm)
{
    glm::mat4 pvm = projectionMatrix * viewMatrix * ctm;
    setMat4("pvm", pvm);
    setMat4("model", ctm);
    setMat4("normalMatrix", glm::transpose(glm::inverse(ctm)));
}

void UniformManager::clearUniforms()
{
    // clear global
    setFloat("k_a", 0.0f);
    setFloat("k_d", 0.0f);
    setFloat("k_s", 0.0f);

    // clear material
    setVec4("material.cAmbient", glm::vec4(0.0f));
    setVec4("material.cDiffuse", glm::vec4(0.0f));
    setVec4("material.cSpecular", glm::vec4(0.0f));
    setFloat("material.shininess", 0.0f);
    
    // clear mvp
    setMat4("model", glm::mat4(1.0f));
    setMat4("view", glm::mat4(1.0f));
    setMat4("projection", glm::mat4(1.0f));
    setMat4("normalMatrix", glm::mat4(1.0f));

    // clear lights
    for (int i = 0; i < 8; i++)
    {
        std::string lightUniformStruct = "lights[" + std::to_string(i) + "]";
        setBool((lightUniformStruct + ".enabled").c_str(), false);
        setVec4((lightUniformStruct + ".color").c_str(), glm::vec4(0.0f));
        setVec4((lightUniformStruct + ".dir").c_str(), glm::vec4(0.0f));
        setVec4((lightUniformStruct + ".pos").c_str(), glm::vec4(0.0f));
    }
}

void UniformManager::setLightUniform(SceneLightData &lightData, const int index)
{
    assert(index < 8 && index >= 0); // We only support 8 lights
    std::string lightUniformStruct = "lights[" + std::to_string(index) + "]";
    std::string lightType = lightUniformStruct + ".type";
    setBool((lightUniformStruct + ".enabled").c_str(), true);
    switch(lightData.type)
    {
        case LightType::LIGHT_DIRECTIONAL:
            setInt(lightType.c_str(), 0);
            break;
        case LightType::LIGHT_POINT:
            setInt(lightType.c_str(), 1);
            break;
        case LightType::LIGHT_SPOT:
            setInt(lightType.c_str(), 2);
            break;
    }
    setVec4((lightUniformStruct + ".color").c_str(), lightData.color);
    setVec3((lightUniformStruct + ".function").c_str(), lightData.function);
    setVec4((lightUniformStruct + ".pos").c_str(), lightData.pos);
    setVec4((lightUniformStruct + ".dir").c_str(), lightData.dir);
    setFloat((lightUniformStruct + ".penumbra").c_str(), lightData.penumbra);
    setFloat((lightUniformStruct + ".angle").c_str(), lightData.angle);
}

void UniformManager::setMaterialUniform(const SceneMaterial &material)
{
    setVec4("material.cAmbient", material.cAmbient);
    setVec4("material.cDiffuse", material.cDiffuse);
    setVec4("material.cSpecular", material.cSpecular);
    setFloat("material.shininess", material.shininess);
    setFloat("material.blend", material.blend);

    setBool("filemap.isUsed", material.textureMap.isUsed);
    setFloat("filemap.repeatU", material.textureMap.repeatU);
    setFloat("filemap.repeatV", material.textureMap.repeatV);
}


void UniformManager::setTextureUniform(GLuint slot)
{
    setInt("textureSampler", slot);
}

void UniformManager::setTextImageUniform(GLuint slot)
{
    setInt("textImgSampler", slot);
}

void UniformManager::setPostProcessUniforms(bool usePixelFilter, bool useBlurFilter, bool useFXAA, bool useGrayscale)
{
    setBool("usePixelFilter", usePixelFilter);
    setBool("useBlurFilter", useBlurFilter);
    setBool("useFXAAFilter", useFXAA);
    setBool("useGrayscaleFilter", useGrayscale);
}