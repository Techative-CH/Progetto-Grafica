#include "node.h"

#include <glm/gtc/matrix_transform.hpp>

Eng::Node::Node(const std::string& name) :
	Object { name },
	parent { nullptr },
	localMatrix { 1.0f }
{
}


Eng::Node::~Node()
{
	for (Node* child : children)
		delete child;
}

void Eng::Node::addChild(Node* child)
{
	if (child == nullptr)
		return;

	child->parent = this;

	children.push_back(child);
}

const std::vector<Eng::Node*>& Eng::Node::getChildren() const
{
	return children;
}

Eng::Node* Eng::Node::getParent() const
{
	return parent;
}

void Eng::Node::setLocalMatrix(const glm::mat4& matrix)
{
	localMatrix = matrix;
}

glm::mat4 Eng::Node::getLocalMatrix() const
{
	return localMatrix;
}

glm::mat4 Eng::Node::getWorldMatrix() const
{
	if (parent != nullptr)
		return parent->getWorldMatrix() * getLocalMatrix();

	return getLocalMatrix();
}

void Eng::Node::render()
{

}