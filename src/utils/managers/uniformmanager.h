#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include "utils/sceneparser.h"

class UniformManager {
private:
    // std::unordered_map<std::string, GLint> m_uniformLocations;
    GLuint m_shader;

public:
    UniformManager(GLuint shaderProgram) : m_shader(shaderProgram) {}

    void setShader(GLuint shaderProgram) {
        m_shader = shaderProgram;
        glUseProgram(m_shader);
    }

    // Cache uniform locations
    GLuint getUniformLocation(const char* name);

    // Type-safe uniform setters
    void setFloat(const char* name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void setVec3(const char* name, const glm::vec3& value) {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec4(const char* name, const glm::vec4& value) {
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    void setMat4(const char* name, const glm::mat4& value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }

    void setInt(const char* name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }
    
    void setBool(const char* name, bool value) {
        glUniform1i(getUniformLocation(name), value);
    }

    // Actual uniform setters
    void setSceneGlobalUniform(SceneGlobalData& globalData);

    void setMVPUniform(const glm::mat4 &projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4 &ctm);
    
    void setLightUniform(SceneLightData& lightData, const int index);
    
    void setMaterialUniform(const SceneMaterial& material);

    void setCameraPosUniform(const glm::vec4& cameraPos) {
        setVec4("camPos", cameraPos);
    }
    
    void setTextureUniform(GLuint slot=0);

    void setPostProcessUniforms(bool usePixelFilter, bool useBlurFilter, bool useFXAA, bool useGrayscale);
    void clearUniforms();

    void setTextImageUniform(GLuint slot);
};
