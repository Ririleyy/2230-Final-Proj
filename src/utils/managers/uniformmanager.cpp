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

void UniformManager::setMVPUniform(const glm::mat4 &projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4 &ctm)
{
    glm::mat4 pvm = projectionMatrix * viewMatrix * ctm;
    setMat4("pvm", pvm);
    setMat4("model", ctm);
    setMat4("normalMatrix", glm::transpose(glm::inverse(ctm)));
}

void UniformManager::clearUniforms()
{
    // clear mvp
    setMat4("model", glm::mat4(1.0f));
    setMat4("view", glm::mat4(1.0f));
    setMat4("projection", glm::mat4(1.0f));
    setMat4("normalMatrix", glm::mat4(1.0f));
}

void UniformManager::setTextureUniform(GLuint slot)
{
    setInt("textureSampler", slot);
}

void UniformManager::setTextImageUniform(GLuint slot)
{
    setInt("textImgSampler", slot);
}
