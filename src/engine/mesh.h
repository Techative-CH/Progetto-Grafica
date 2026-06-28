#pragma once

#include "node.h"

namespace Eng
{
	class Mesh : public Node
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