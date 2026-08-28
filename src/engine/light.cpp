#include "light.h"

Eng::Light::Light(const std::string& name)
	: Node{ name }
{
}

Eng::Light::~Light()
{
}

void Eng::Light::render()
{
	// Light rendering will be implemented later.
}