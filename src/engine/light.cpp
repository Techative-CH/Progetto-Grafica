#include "light.h"

Eng::Light::Light(
    const std::string& name,
    const glm::vec3& color)
    : Node{ name },
    color{ color },
    castShadows{ false },
    volumetric{ false }
{
}

Eng::Light::~Light()
{
}

void Eng::Light::setColor(
    float r,
    float g,
    float b)
{
    color = glm::vec3(r, g, b);
}

const glm::vec3& Eng::Light::getColor() const
{
    return color;
}

void Eng::Light::setCastShadows(bool value)
{
    castShadows = value;
}

bool Eng::Light::getCastShadows() const
{
    return castShadows;
}

void Eng::Light::setVolumetric(bool value)
{
    volumetric = value;
}

bool Eng::Light::getVolumetric() const
{
    return volumetric;
}