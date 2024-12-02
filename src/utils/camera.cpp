#include "camera.h"
#include <stdexcept>

float Camera::getAspectRatio() const
{
  // Optional TODO: implement the getter or make your own design
  throw std::runtime_error("not implemented");
}

// float Camera::getHeightAngle() const {
//     // Optional TODO: implement the getter or make your own design
//     return
// }

// float Camera::getWidthAngle(float heightAngle, int width, int height) const {
//     // Optional TODO: implement the getter or make your own design
//     throw std::runtime_error("not implemented");
// }

// float Camera::getFocalLength() const {
//     // Optional TODO: implement the getter or make your own design
//     throw std::runtime_error("not implemented");
// }

// float Camera::getAperture() const {
//     // Optional TODO: implement the getter or make your own design
//     throw std::runtime_error("not implemented");
// }

glm::vec3 Camera::getCameraPos() const { return camPos; }

Camera::Camera(const SceneCameraData &camData, const Settings &settings, int height, const int width)
{
  updateCamera(camData, settings, height, width);
}

void Camera::updateCamera(const SceneCameraData &camData, const Settings &settings, const int height,
                          const int width)
{
  w = -glm::normalize(glm::vec3(camData.look));
  v = glm::vec3(camData.up) - glm::dot(glm::vec3(camData.up), w) * w;
  v = glm::normalize(v);
  u = glm::cross(v, w);
  nearPlane = settings.nearPlane;
  farPlane = settings.farPlane;
  camPos = camData.pos;
  heightAngle = camData.heightAngle;
  aspectRatio = static_cast<float>(width) / height;

  glm::mat4 rotationMatrix =
      glm::mat4(u.x, v.x, w.x, 0.0f, u.y, v.y, w.y, 0.0f, u.z, v.z, w.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 translationMatrix = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      -camData.pos.x, -camData.pos.y, -camData.pos.z, 1.0f);
  viewMat = rotationMatrix * translationMatrix;
  invViewMat = glm::inverse(viewMat);

  calcProjMat();
}

// aspect = width / height
// https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
void Camera::calcProjMat()
{
  float tanThetaHalf = glm::tan(glm::radians(heightAngle / 2)); // (theta / 2) / 2 *pi
  projMat = glm::mat4(
      1.0f / (aspectRatio * tanThetaHalf), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / tanThetaHalf, 0.0f, 0.0f,
      0.0f, 0.0f, -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
      0.0f, 0.0f, -2.0f * farPlane * nearPlane / (farPlane - nearPlane), 0.0f);
}

void Camera::updateAspectRatio(const float aspectRatio)
{
  this->aspectRatio = aspectRatio;
  calcProjMat();
}

void Camera::updateNearFar(const float near, const float far)
{
  this->nearPlane = near;
  this->farPlane = far;
  calcProjMat();
}

glm::mat4 Camera::getViewMatrix(const SceneCameraData &camData)
{
  glm::vec3 w = -glm::normalize(glm::vec3(camData.look));
  glm::vec3 v = glm::vec3(camData.up) - glm::dot(glm::vec3(camData.up), w) * w;
  v = glm::normalize(v);
  glm::vec3 u = glm::cross(v, w);

  glm::mat4 rotationMatrix = glm::mat4(
      u.x, v.x, w.x, 0.0f,
      u.y, v.y, w.y, 0.0f,
      u.z, v.z, w.z, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 translationMatrix = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      -camData.pos.x, -camData.pos.y, -camData.pos.z, 1.0f);
  return rotationMatrix * translationMatrix;
}

glm::mat4 Camera::getProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar)
{
  float c = -zNear / zFar;
  float tan_h = tan(fovy / 2);
  float tan_w = tan_h * aspect;
  const glm::mat4 openglMat = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, -2.0f, 0.0f,
      0.0f, 0.0f, -1.0f, 1.0f);

  glm::mat4 unhingeMat = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f / (1.0f + c), -1.0f,
      0.0f, 0.0f, -c / (1.0f + c), 0.0f);
    
  glm::mat4 scaleMat = glm::mat4(
      1.0f / (zFar * tan_w), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / (zFar * tan_h), 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f / zFar, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);

  return openglMat * unhingeMat * scaleMat;
  // float f = 1 / glm::tan(glm::radians(fovy / 2));
  // return glm::mat4(
  //     f / aspect, 0.0f, 0.0f, 0.0f,
  //     0.0f, f, 0.0f, 0.0f,
  //     0.0f, 0.0f, (zFar + zNear) / (zNear - zFar), -1.0f,
  //     0.0f, 0.0f, 2 * zFar * zNear / (zNear - zFar), 0.0f);
}

float Camera::getObjectDistance(const glm::mat4 &ctm, const glm::mat4 &invView)
{
    glm::vec3 cameraPos = glm::vec3(invView[3]); // Last column contains position
    glm::vec3 objectPos = glm::vec3(ctm[3]); // Last column of CTM

    // Calculate distance using glm::distance
    return glm::distance(objectPos, cameraPos);
}

glm::mat3 Camera::getRotationMatrix(const glm::vec3 &axis, const float angle)
{
  return glm::mat3(glm::cos(angle) + (1 - glm::cos(angle)) * axis.x * axis.x,
                   (1 - glm::cos(angle)) * axis.x * axis.y - axis.z * glm::sin(angle),
                   (1 - glm::cos(angle)) * axis.x * axis.z + axis.y * glm::sin(angle),
                   (1 - glm::cos(angle)) * axis.x * axis.y + axis.z * glm::sin(angle),
                   glm::cos(angle) + (1 - glm::cos(angle)) * axis.y * axis.y,
                   (1 - glm::cos(angle)) * axis.y * axis.z - axis.x * glm::sin(angle),
                   (1 - glm::cos(angle)) * axis.x * axis.z - axis.y * glm::sin(angle),
                   (1 - glm::cos(angle)) * axis.y * axis.z + axis.x * glm::sin(angle),
                   glm::cos(angle) + (1 - glm::cos(angle)) * axis.z * axis.z);
}

/*
  const glm::mat4 openglMat = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, -2.0f, 0.0f,
      0.0f, 0.0f, -1.0f, 1.0f);

  glm::mat4 unhingeMat = glm::mat4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f / (1.0f + c), -1.0f,
      0.0f, 0.0f, -c / (1.0f + c), 0.0f);

  glm::mat4 scaleMat = glm::mat4(
      1.0f / (zFar * tan_w), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / (zFar * tan_h), 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f / zFar, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);*/