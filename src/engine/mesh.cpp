#include "mesh.h"
#include "engine.h"

Eng::Mesh::Mesh(const std::string& name) : 
	Node{ name },
	cubeEdge{ 1.0f },
	texture{ nullptr }
{}

Eng::Mesh::~Mesh()
{}

void Eng::Mesh::setCubeEdge(float edge)
{
	cubeEdge = edge;
}

void Eng::Mesh::setTexture(Texture* texture)
{
	this->texture = texture;
}

Eng::Texture* Eng::Mesh::getTexture() const
{
	return texture;
}

void Eng::Mesh::render()
{
	if (texture != nullptr)
		texture->bind();

	Eng::Base::getInstance().drawCube(cubeEdge);

	if (texture != nullptr)
		texture->unbind();
}