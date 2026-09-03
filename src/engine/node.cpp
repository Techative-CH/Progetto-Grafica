#include "node.h"

#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Creates a node with no parent and an identity local transformation.
 * @param name Name of the node
 */
Eng::Node::Node(const std::string& name) :
	Object{ name },
	parent{ nullptr },
	localMatrix{ 1.0f }
{}

/**
 * @brief Destroys the node and all its children.
 */
Eng::Node::~Node()
{
	for (Node* child : children)
		delete child;
}

/**
 * @brief Adds a child node and sets this node as its parent.
 * @param child The child node to add
 */
void Eng::Node::addChild(Node* child)
{
	if (child == nullptr)
		return;

	child->parent = this;

	children.push_back(child);
}

/**
 * @brief Returns the child nodes of this node.
 * @return The vector of child nodes
 */
const std::vector<Eng::Node*>& Eng::Node::getChildren() const
{
	return children;
}

/**
 * @brief Returns the parent of this node.
 * @return The parent node
 */
Eng::Node* Eng::Node::getParent() const
{
	return parent;
}

/**
 * @brief Sets the local transformation matrix of the node.
 * @param matrix Local transformation matrix
 */
void Eng::Node::setLocalMatrix(const glm::mat4& matrix)
{
	localMatrix = matrix;
}

/**
 * @brief Returns the local transformation matrix of the node.
 * @return The local transformation matrix
 */
glm::mat4 Eng::Node::getLocalMatrix() const
{
	return localMatrix;
}

/**
 * @brief Calculates and returns the world transformation matrix of the node.
 * @return The world transformation matrix
 */
glm::mat4 Eng::Node::getWorldMatrix() const
{
	if (parent != nullptr)
		return parent->getWorldMatrix() * getLocalMatrix();

	// root node with no parent transformation
	return getLocalMatrix();
}

/**
 * @brief Recursively searches this node and its children by name.
 * @param name Name of the node to find
 * @return The matching node, or nullptr if not found
 */
Eng::Node* Eng::Node::findByName(const std::string& name)
{
	if (getName() == name)
		return this;

	// Search recursively inside each child tree
	for (Node* child : children)
	{
		Node* result = child->findByName(name);

		if (result != nullptr)
			return result;
	}

	return nullptr;
}

/**
 * @brief Renders the node.
 */
void Eng::Node::render()
{}