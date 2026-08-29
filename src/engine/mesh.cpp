#include "mesh.h"
#include "engine.h"

#include <GL/freeglut.h>

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

void Eng::Mesh::setVertices(const std::vector<Vertex>& value)
{
	vertices = value;
}

void Eng::Mesh::setIndices(const std::vector<unsigned int>& value)
{
	indices = value;
}

const std::vector<Eng::Vertex>& Eng::Mesh::getVertices() const
{
	return vertices;
}

const std::vector<unsigned int>& Eng::Mesh::getIndices() const
{
	return indices;
}

void Eng::Mesh::render()
{
    if (material != nullptr)
        material->render();

    if (!vertices.empty() && !indices.empty())
    {
        glBegin(GL_TRIANGLES);

        for (unsigned int index : indices)
        {
            if (index >= vertices.size())
                continue;

            const Vertex& vertex = vertices[index];

            glNormal3f(
                vertex.normal.x,
                vertex.normal.y,
                vertex.normal.z
            );

            glTexCoord2f(
                vertex.texCoord.x,
                vertex.texCoord.y
            );

            glVertex3f(
                vertex.position.x,
                vertex.position.y,
                vertex.position.z
            );
        }

        glEnd();
    }
    else
    {
        Base::getInstance().drawCube(cubeEdge);
    }

    if (material != nullptr)
        material->unbind();
}