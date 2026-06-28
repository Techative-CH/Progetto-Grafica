#include "node.h"

#include <glm/gtc/matrix_transform.hpp>

Eng::Node::Node(const std::string& name) :
	Object { name },
	position { 0.0f, 0.0f, 0.0f },
	rotationAxis { 0.0f, 1.0f, 0.0f },
	rotationAngle { 0.0f },
	scale { 1.0f, 1.0f, 1.0f }
{ }


Eng::Node::~Node()
{
	for (Node* child : children)
		delete child;
}

void Eng::Node::addChild(Node* child)
{
	children.push_back(child);
}

const std::vector<Eng::Node*>& Eng::Node::getChildren() const
{
	return children;
}

void Eng::Node::setPosition(float x, float y, float z)
{
	position = glm::vec3(x, y, z);
}

void Eng::Node::setRotation(float angle, float x, float y, float z)
{
	rotationAngle = angle;
	rotationAxis = glm::vec3(x, y, z);
}

void Eng::Node::setScale(float x, float y, float z)
{
	scale = glm::vec3(x, y, z);
}

glm::mat4 Eng::Node::getLocalMatrix() const
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix = glm::translate(matrix, position);
	matrix = glm::rotate(matrix, glm::radians(rotationAngle), rotationAxis);
	matrix = glm::scale(matrix, scale);

	return matrix;
}

void Eng::Node::render()
{

}