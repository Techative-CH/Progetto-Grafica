#pragma once

#include "engineApi.h"
#include "object.h"
#include "node.h"

#include <vector>

#include <glm/glm.hpp>

namespace Eng
{
	struct RenderElement
	{
		Node* node;
		glm::mat4 worldMatrix;
	};

	class ENG_API List : public Object
	{
	public:
		List(const std::string& name);
		virtual ~List();

		void add(Node* node, const glm::mat4& worldMatrix);
		void clear();

		std::size_t size() const;
		const RenderElement* get(std::size_t index) const;

		void pass(Node* root);

	private:
		std::vector<RenderElement> elements;

		void passRecursive(Node* node);
	};
}