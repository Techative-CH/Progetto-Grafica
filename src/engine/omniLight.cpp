/**
 * @file omniLight.cpp
 * @brief Omnidirectional light file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#include "omniLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Creates an omnidirectional light with the specified color.
 * @param name Name of the light
 * @param color RGB color of the light
 */
Eng::OmniLight::OmniLight(const std::string& name, const glm::vec3& color)
    : Light{ name, color },
    radius{ 0.0f }
{}

/**
 * @brief Sets the radius of the omnidirectional light.
 * @param radius Radius of the light
 */
void Eng::OmniLight::setRadius(float radius)
{
    this->radius = radius;
}

/**
 * @brief Returns the radius of the omnidirectional light.
 * @return Radius of the light
 */
float Eng::OmniLight::getRadius() const
{
    return radius;
}

/**
 * @brief Configures and renders the omnidirectional light.
 * @param worldMatrix World transformation matrix of the light
 * @param viewMatrix View transformation matrix of the active camera
 * @param lightId Identifier of to the light
 */
void Eng::OmniLight::renderLight(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, unsigned int lightId)
{
    GLenum id = static_cast<GLenum>(lightId);

    // Point light located at the origin of its local coordinate system
    glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f); // w = 1 -> positional light

    // Convert the light color to RGBA array
    GLfloat diffuse[] = { color.r, color.g, color.b, 1.0f };

    // Transform the light from local space to camera space
    glm::mat4 modelViewMatrix = viewMatrix * worldMatrix;

    // Load the model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelViewMatrix));

    // Configure the position and color of the light
    glLightfv(id, GL_POSITION, glm::value_ptr(position));
    glLightfv(id, GL_DIFFUSE, diffuse);
    glLightfv(id, GL_SPECULAR, diffuse);

    // Make the light omnidirectional instead of a spotlight
    glLightf(id, GL_SPOT_CUTOFF, 180.0f); // 180 = not a spotlight, light in every direction
}