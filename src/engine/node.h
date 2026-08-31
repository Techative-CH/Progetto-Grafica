#pragma once

#include "engineApi.h"
#include "object.h"

#include <vector>
#include <string>

#include <glm/glm.hpp>

namespace Eng {
	class ENG_API Node : public Object
	{
	public:
		Node(const std::string& name);
		virtual ~Node();

		void addChild(Node* child);
		const std::vector<Node*>& getChildren() const;

		Node* getParent() const;

		void setLocalMatrix(const glm::mat4& matrix);
		glm::mat4 getLocalMatrix() const;
		glm::mat4 getWorldMatrix() const;

		Node* findByName(const std::string& name);

		virtual void render();

	private:
		Node* parent;
		std::vector<Node*> children;
		glm::mat4 localMatrix;
	};
}