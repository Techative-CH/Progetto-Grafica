/**
 * @file mesh.h
 * @brief Mesh include file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once

#include "engineApi.h"
#include "node.h"
#include "material.h"

#include <vector>

#include <glm/glm.hpp>

namespace Eng
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	class ENG_API Mesh : public Node
	{
	public:
		Mesh(const std::string& name);
		virtual ~Mesh();

		void setMaterial(Material* material);
		Material* getMaterial() const;

		void setVertices(const std::vector<Vertex>& vertices);
		void setIndices(const std::vector<unsigned int>& indices);

		const std::vector<Vertex>& getVertices() const;
		const std::vector<unsigned int>& getIndices() const;

		void render() override;

	private:
		Material* material;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}
