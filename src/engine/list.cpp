#include "list.h"
#include "node.h"

Eng::List::List(const std::string& name)
	: Object{ name }
{
}

Eng::List::~List()
{
}

void Eng::List::add(Node* node)
{
	if (node != nullptr)
		nodes.push_back(node);
}

void Eng::List::clear()
{
	nodes.clear();
}

std::size_t Eng::List::size() const
{
	return nodes.size();
}

Eng::Node* Eng::List::get(std::size_t index) const
{
	if (index >= nodes.size())
		return nullptr;

	return nodes[index];
}