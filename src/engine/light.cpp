#include "light.h"

Eng::Light::Light(const std::string& name)
	: Node{ name },
	color{ 1.0f, 1.0f, 1.0f }
{
}

Eng::Light::~Light()
{
}

void Eng::Light::setColor(float r, float g, float b)
{
	color = glm::vec3(r, g, b);
}

const glm::vec3& Eng::Light::getColor() const
{
	return color;
}

void Eng::Light::render()
{
	// Light rendering will be implemented later.
}