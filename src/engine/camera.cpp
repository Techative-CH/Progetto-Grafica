/**
 * @file camera.cpp
 * @brief Camera file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

/**
 * @brief Creates a camera with the specified name.
 * @param name Name of the camera
 */
Eng::Camera::Camera(const std::string& name)
    : Node{ name },
    projectionMatrix{ 1.0f }
{}

/**
 * @brief Destroys the camera.
 */
Eng::Camera::~Camera()
{}

/**
 * @brief Configures the perspective projection of the camera.
 * @param fov Field of view in degrees
 * @param aspect Aspect ratio of the viewport
 * @param nearPlane Distance of the near clipping plane
 * @param farPlane Distance of the far clipping plane
 */
void Eng::Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
    projectionMatrix = glm::perspective(
        glm::radians(fov),
        aspect,
        nearPlane,
        farPlane
    );
}

/**
 * @brief Returns the projection matrix of the camera.
 * @return Constant reference to the projection matrix
 */
const glm::mat4& Eng::Camera::getProjectionMatrix() const
{
    return projectionMatrix;
}

/**
 * @brief Calculates and returns the view matrix of the camera.
 * @return View matrix of the camera
 */
glm::mat4 Eng::Camera::getViewMatrix() const
{
    // The view matrix is the inverse of the camera world transformation
    glm::mat4 worldMatrix = getWorldMatrix();
    return glm::inverse(worldMatrix);
}

/**
 * @brief Renders the camera.
 */
void Eng::Camera::render()
{}