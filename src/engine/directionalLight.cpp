#include "directionalLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Creates a directional light with the specified color.
 * @param name Name of the light
 * @param color RGB color of the light
 */
Eng::DirectionalLight::DirectionalLight(const std::string& name, const glm::vec3& color)
    : Light{ name, color },
    direction{ 0.0f, 0.0f, -1.0f }
{}

/**
 * @brief Sets the direction of the directional light.
 * @param direction Direction of the light
 */
void Eng::DirectionalLight::setDirection(const glm::vec3& direction)
{
    this->direction = direction;
}

/**
 * @brief Returns the direction of the directional light.
 * @return Direction of the light
 */
const glm::vec3& Eng::DirectionalLight::getDirection() const
{
    return direction;
}

/**
 * @brief Configures and renders the directional light.
 * @param worldMatrix World transformation matrix of the light
 * @param viewMatrix View transformation matrix of the active camera
 * @param lightId Identifier of the light
 */
void Eng::DirectionalLight::renderLight(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, unsigned int lightId)
{
    GLenum id = static_cast<GLenum>(lightId);

    // Directional light represented as a vector pointing towards the light source
    glm::vec4 position(-direction.x, -direction.y, -direction.z, 0.0f); // w = 0 -> directional light

    // Convert the light color to RGBA array
    GLfloat diffuse[] = { color.r, color.g, color.b, 1.0f };

    // Transform the light from local space to camera space
    glm::mat4 modelViewMatrix = viewMatrix * worldMatrix;

    // Load the model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelViewMatrix));

    // Configure the direction and color of the light
    glLightfv(id, GL_POSITION, glm::value_ptr(position));
    glLightfv(id, GL_DIFFUSE, diffuse);
    glLightfv(id, GL_SPECULAR, diffuse);

    // Disable the spotlight cone
    glLightf(id, GL_SPOT_CUTOFF, 180.0f);
}