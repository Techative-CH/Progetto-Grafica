#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

Eng::Camera::Camera(const std::string& name)
    : Node{ name },
    projectionMatrix{ 1.0f }
{
}

Eng::Camera::~Camera()
{
}

void Eng::Camera::setPerspective(
    float fov,
    float aspect,
    float nearPlane,
    float farPlane)
{
    projectionMatrix = glm::perspective(
        glm::radians(fov),
        aspect,
        nearPlane,
        farPlane
    );
}

const glm::mat4& Eng::Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

glm::mat4 Eng::Camera::getViewMatrix() const
{
    return glm::inverse(getWorldMatrix());
}

void Eng::Camera::render()
{
}