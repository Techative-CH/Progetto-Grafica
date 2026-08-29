#include "list.h"
#include "node.h"
#include "mesh.h"
#include "light.h"

Eng::List::List(const std::string& name)
	: Object{ name }
{
}

Eng::List::~List()
{
}

void Eng::List::add(Node* node, const glm::mat4& worldMatrix)
{
	if (node != nullptr)
		elements.push_back({ node, worldMatrix });
}

void Eng::List::clear()
{
	elements.clear();
}

std::size_t Eng::List::size() const
{
	return elements.size();
}

const Eng::RenderElement* Eng::List::get(std::size_t index) const
{
	if (index >= elements.size())
		return nullptr;

	return &elements[index];
}

void Eng::List::pass(Node* root)
{
	clear();

	if (root != nullptr)
		passRecursive(root);
}

void Eng::List::passRecursive(Node* node)
{
	if (node == nullptr)
		return;

	if (dynamic_cast<Mesh*>(node) != nullptr ||
		dynamic_cast<Light*>(node) != nullptr)
	{
		add(node, node->getWorldMatrix());
	}

	for (Node* child : node->getChildren())
		passRecursive(child);
}