#include "light.h"

Eng::Light::Light(const std::string& name) :
    Node{ name },
    subtype{ Subtype::OMNI },
    color{ 1.0f, 1.0f, 1.0f },
    radius{ 0.0f },
    direction{ 0.0f, 0.0f, -1.0f },
    cutoff{ 180.0f },
    spotExponent{ 0.0f },
    castShadows{ false },
    volumetric{ false }
{
}

Eng::Light::~Light()
{
}

void Eng::Light::setSubtype(Subtype value)
{
    subtype = value;
}

Eng::Light::Subtype Eng::Light::getSubtype() const
{
    return subtype;
}

void Eng::Light::setColor(float r, float g, float b)
{
    color = glm::vec3(r, g, b);
}

const glm::vec3& Eng::Light::getColor() const
{
    return color;
}

void Eng::Light::setRadius(float value)
{
    radius = value;
}

float Eng::Light::getRadius() const
{
    return radius;
}

void Eng::Light::setDirection(const glm::vec3& value)
{
    direction = value;
}

const glm::vec3& Eng::Light::getDirection() const
{
    return direction;
}

void Eng::Light::setCutoff(float value)
{
    cutoff = value;
}

float Eng::Light::getCutoff() const
{
    return cutoff;
}

void Eng::Light::setSpotExponent(float value)
{
    spotExponent = value;
}

float Eng::Light::getSpotExponent() const
{
    return spotExponent;
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

void Eng::Light::render()
{
}