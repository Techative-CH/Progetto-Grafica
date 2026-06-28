#include "mesh.h"
#include "engine.h"

Eng::Mesh::Mesh(const std::string& name)
	: Node{ name }, cubeEdge{ 1.0f }
{}

Eng::Mesh::~Mesh()
{}

void Eng::Mesh::setCubeEdge(float edge)
{
	cubeEdge = edge;
}

void Eng::Mesh::render()
{
	Eng::Base::getInstance().drawCube(cubeEdge);
}