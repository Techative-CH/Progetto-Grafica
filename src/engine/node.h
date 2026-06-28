#pragma once

#include "object.h"
#include <vector>

#include <glm/glm.hpp>

namespace Eng {
	class Node : public Object
	{
	public:
		Node(const std::string& name);
		virtual ~Node();

		void addChild(Node* child);
		const std::vector<Node*>& getChildren() const;

		void setPosition(float x, float y, float z);
		void setRotation(float angle, float x, float y, float z);
		void setScale(float x, float y, float z);

		glm::mat4 getLocalMatrix() const;

		virtual void render();

	private:
		std::vector<Node*> children;
		glm::vec3 position;
		glm::vec3 rotationAxis;
		float rotationAngle;
		glm::vec3 scale;
	};
}