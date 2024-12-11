#pragma once

#include <glm/glm.hpp>

// A class representing a virtual camera.
class Camera
{
public:
    glm::vec3 m_eye;
    glm::vec3 m_look;
    glm::vec3 m_up; 
    // Camera() = default;
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    static glm::mat3 getRotationMatrix(const glm::vec3& axis, const float angle);

private:
};
