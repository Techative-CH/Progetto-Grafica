#pragma once

#include "engineApi.h"
#include "node.h"

#include <glm/glm.hpp>

namespace Eng
{
	class ENG_API Light : public Node
	{
	public:
		Light(const std::string& name);
		virtual ~Light();

		void setColor(float r, float g, float b);
		const glm::vec3& getColor() const;

		void render() override;
	
	private:
		glm::vec3 color;
	};
}