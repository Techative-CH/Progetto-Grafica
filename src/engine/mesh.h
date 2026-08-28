#pragma once

#include "engineApi.h"
#include "node.h"
#include "material.h"

namespace Eng
{
	class ENG_API Mesh : public Node
	{
	public:
		Mesh(const std::string& name);
		virtual ~Mesh();

		void setCubeEdge(float edge);

		void setMaterial(Material* material);
		Material* getMaterial() const;

		void render() override;

	private:
		float cubeEdge;
		Material* material;
	};
}
