#include "mesh.h"
#include "engine.h"

Eng::Mesh::Mesh(const std::string& name) : 
	Node{ name },
	cubeEdge{ 1.0f },
	material{ nullptr }
{}

Eng::Mesh::~Mesh()
{}

void Eng::Mesh::setCubeEdge(float edge)
{
	cubeEdge = edge;
}

void Eng::Mesh::setMaterial(Material* material)
{
	this->material = material;
}

Eng::Material* Eng::Mesh::getMaterial() const
{
	return material;
}

void Eng::Mesh::render()
{
	Texture* texture = nullptr;

	if (material != nullptr)
		texture = material->getTexture();

	if (texture != nullptr)
		texture->bind();

	Eng::Base::getInstance().drawCube(cubeEdge);

	if (texture != nullptr)
		texture->unbind();
}