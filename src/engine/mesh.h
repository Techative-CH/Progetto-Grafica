#pragma once

#include "engineApi.h"
#include "node.h"
#include "texture.h"

namespace Eng
{
	class ENG_API Mesh : public Node
	{
	public:
		Mesh(const std::string& name);
		virtual ~Mesh();

		void setCubeEdge(float edge);

		void setTexture(Texture* texture);
		Texture* getTexture() const;

		void render() override;

	private:
		float cubeEdge;
		Texture* texture;
	};
}