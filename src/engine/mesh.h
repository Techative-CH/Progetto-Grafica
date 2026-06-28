#pragma once

#include "engineApi.h"
#include "node.h"

namespace Eng
{
	class ENG_API Mesh : public Node
	{
	public:
		Mesh(const std::string& name);
		virtual ~Mesh();

		void setCubeEdge(float edge);
		void render() override;

	private:
		float cubeEdge;
	};
}