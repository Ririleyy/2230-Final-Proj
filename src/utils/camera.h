#pragma once

#include <glm/glm.hpp>
#include "settings.h"

// A class representing a virtual camera.
struct SceneCameraData
{
    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    float heightAngle; // The height angle of the camera in RADIANS

    float aperture;    // Only applicable for depth of field
    float focalLength; // Only applicable for depth of field
};

class Camera
{
public:
    Camera(const SceneCameraData &camData, const Settings &settings, int height, const int width);
    Camera() = default;
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getInverseViewMatrix() const;
    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;
    glm::vec3 getRayDirection(float x, float y) const;
    glm::vec3 getCameraPos() const;
    static glm::mat4 getViewMatrix(const SceneCameraData &camData);
    static glm::mat4 getProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar);
    static float getObjectDistance(const glm::mat4& ctm, const glm::mat4& invView);
    static glm::mat3 getRotationMatrix(const glm::vec3& axis, const float angle);
    void updateCamera(const SceneCameraData &camData, const Settings &settings, const int height, const int width);
    void updateAspectRatio(const float aspectRatio);
    void updateNearFar(const float near, const float far);

private:
    void calcProjMat();
    float nearPlane, farPlane, heightAngle, aspectRatio;
    glm::vec3 u, v, w, camPos;
    glm::mat4 viewMat, invViewMat, projMat;
};
