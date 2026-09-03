/**
 * @file light.cpp
 * @brief Light main file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#include "light.h"

/**
 * @brief Creates a light with the specified color.
 * @param name Name of the light
 * @param color RGB color of the light
 */
Eng::Light::Light(const std::string& name, const glm::vec3& color)
    : Node{ name },
    color{ color },
    castShadows{ false },
    volumetric{ false }
{}

/**
 * @brief Destroys the light.
 */
Eng::Light::~Light()
{}

/**
 * @brief Sets the color of the light.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 */
void Eng::Light::setColor(float r, float g, float b)
{
    color = glm::vec3(r, g, b);
}

/**
 * @brief Returns the color of the light.
 * @return Constant reference to the RGB color
 */
const glm::vec3& Eng::Light::getColor() const
{
    return color;
}

/**
 * @brief Sets whether the light casts shadows.
 * @param value True to enable shadow casting, false otherwise
 */
void Eng::Light::setCastShadows(bool value)
{
    castShadows = value;
}

/**
 * @brief Returns whether the light casts shadows.
 * @return True if shadow casting is enabled, false otherwise
 */
bool Eng::Light::getCastShadows() const
{
    return castShadows;
}

/**
 * @brief Sets whether the light is volumetric.
 * @param value True to enable volumetric lighting, false otherwise
 */
void Eng::Light::setVolumetric(bool value)
{
    volumetric = value;
}

/**
 * @brief Returns whether the light is volumetric.
 * @return True if volumetric lighting is enabled, false otherwise
 */
bool Eng::Light::getVolumetric() const
{
    return volumetric;
}