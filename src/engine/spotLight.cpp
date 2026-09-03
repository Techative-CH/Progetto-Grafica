#include "spotLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief Creates a spotlight with the specified color.
 * @param name Name of the light
 * @param color RGB color of the light
 */
Eng::SpotLight::SpotLight(const std::string& name, const glm::vec3& color)
    : Light{ name, color },
    radius{ 0.0f },
    direction{ 0.0f, 0.0f, -1.0f },
    cutoff{ 180.0f },
    spotExponent{ 0.0f }
{}

/**
 * @brief Sets the radius of the spotlight.
 * @param radius Radius of the light
 */
void Eng::SpotLight::setRadius(float radius)
{
    this->radius = radius;
}

/**
 * @brief Returns the radius of the spotlight.
 * @return Radius of the light
 */
float Eng::SpotLight::getRadius() const
{
    return radius;
}

/**
 * @brief Sets the direction of the spotlight.
 * @param direction Direction of the light
 */
void Eng::SpotLight::setDirection(const glm::vec3& direction)
{
    this->direction = direction;
}

/**
 * @brief Returns the direction of the spotlight.
 * @return Direction of the light
 */
const glm::vec3& Eng::SpotLight::getDirection() const
{
    return direction;
}

/**
 * @brief Sets the cutoff angle of the spotlight.
 * @param cutoff Cutoff angle of the light
 */
void Eng::SpotLight::setCutoff(float cutoff)
{
    this->cutoff = cutoff;
}

/**
 * @brief Returns the cutoff angle of the spotlight.
 * @return Cutoff angle of the light
 */
float Eng::SpotLight::getCutoff() const
{
    return cutoff;
}

/**
 * @brief Sets the concentration exponent of the spotlight.
 * @param exponent Spotlight concentration exponent
 */
void Eng::SpotLight::setSpotExponent(float exponent)
{
    spotExponent = exponent;
}

/**
 * @brief Returns the concentration exponent of the spotlight.
 * @return Spotlight concentration exponent
 */
float Eng::SpotLight::getSpotExponent() const
{
    return spotExponent;
}

/**
 * @brief Configures and renders the spotlight.
 * @param worldMatrix World transformation matrix of the light
 * @param viewMatrix View transformation matrix of the active camera
 * @param lightId Identifier of the light
 */
void Eng::SpotLight::renderLight(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, unsigned int lightId)
{
    GLenum id = static_cast<GLenum>(lightId);

    // Point light located at the origin of its local coordinate system
    glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f); // w = 1 -> positional light

    // Convert the light color to RGBA array
    GLfloat diffuse[] = { color.r, color.g, color.b, 1.0f };

    // Direction of the spotlight in its local coordinate system
    glm::vec3 spotDirection = direction;

    // Transform the light from local space to camera space
    glm::mat4 modelViewMatrix = viewMatrix * worldMatrix;

    // Load the model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(modelViewMatrix));

    // Configure the position and direction of the light
    glLightfv(id, GL_POSITION, glm::value_ptr(position));
    glLightfv(id, GL_SPOT_DIRECTION, glm::value_ptr(spotDirection)); // Direction in which the cone is pointing

    // Configure the spotlight cone
    glLightf(id, GL_SPOT_CUTOFF, cutoff); // Defines how wide the cone is
    glLightf(id, GL_SPOT_EXPONENT, spotExponent); // Defines how concentrated the light is towards the center

    // Configure the color of the light
    glLightfv(id, GL_DIFFUSE, diffuse);
    glLightfv(id, GL_SPECULAR, diffuse);
}