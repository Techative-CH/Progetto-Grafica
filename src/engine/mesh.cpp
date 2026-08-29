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
	if (material != nullptr)
		material->render();

	Eng::Base::getInstance().drawCube(cubeEdge);

	if (material != nullptr)
		material->unbind();
}