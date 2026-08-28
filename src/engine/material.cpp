#include "material.h"
#include "texture.h"

Eng::Material::Material(const std::string& name)
	: Object{ name },
	texture{ nullptr }
{
}

Eng::Material::~Material()
{
}

void Eng::Material::setTexture(Texture* texture)
{
	this->texture = texture;
}

Eng::Texture* Eng::Material::getTexture() const
{
	return texture;
}