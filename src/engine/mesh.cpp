#include "mesh.h"
#include "engine.h"

#include <GL/freeglut.h>


/**
 * @brief Creates a mesh with no material assigned.
 * @param name Name of the mesh
 */
Eng::Mesh::Mesh(const std::string& name) :
    Node{ name },
    material{ nullptr }
{}

/**
 * @brief Destroys the mesh.
 */
Eng::Mesh::~Mesh()
{}

/**
 * @brief Assigns a material to the mesh.
 * @param material The material to assign
 */
void Eng::Mesh::setMaterial(Material* material)
{
    this->material = material;
}

/**
 * @brief Returns the material assigned to the mesh.
 * @return The assigned material
 */
Eng::Material* Eng::Mesh::getMaterial() const
{
    return material;
}

/**
 * @brief Sets the vertices of the mesh.
 * @param value Vector containing the mesh vertices
 */
void Eng::Mesh::setVertices(const std::vector<Vertex>& value)
{
    vertices = value;
}

/**
 * @brief Sets the indices used to render the mesh.
 * @param value Vector containing the vertex indices
 */
void Eng::Mesh::setIndices(const std::vector<unsigned int>& value)
{
    indices = value;
}

/**
 * @brief Returns the vertices of the mesh.
 * @return The vertex vector
 */
const std::vector<Eng::Vertex>& Eng::Mesh::getVertices() const
{
    return vertices;
}

/**
 * @brief Returns the indices of the mesh.
 * @return The index vector
 */
const std::vector<unsigned int>& Eng::Mesh::getIndices() const
{
    return indices;
}

/**
 * @brief Renders the mesh using its material, vertices and indices.
 */
void Eng::Mesh::render()
{
    // Apply the material before rendering the geometry
    if (material != nullptr)
        material->render();

    if (!vertices.empty() && !indices.empty())
    {
        glBegin(GL_TRIANGLES);

        // Each index identifies the vertex to render
        for (unsigned int index : indices)
        {
            // Ignore invalid index
            if (index >= vertices.size())
                continue;

            const Vertex& vertex = vertices[index];

            glNormal3f(vertex.normal.x, vertex.normal.y, vertex.normal.z); // Normal for lighting
            glTexCoord2f(vertex.texCoord.x, vertex.texCoord.y); // Coordinates UV for texture mapping
            glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z); // Position
        }

        glEnd();
    }
}