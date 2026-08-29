#pragma once

#include "engineApi.h"
#include "object.h"
#include "node.h"

#include <vector>

namespace Eng
{
	class ENG_API List : public Object
	{
	public:
		List(const std::string& name);
		virtual ~List();

		void add(Node* node);
		void clear();

		std::size_t size() const;
		Node* get(std::size_t index) const;

	private:
		std::vector<Node*> nodes;
	};
}