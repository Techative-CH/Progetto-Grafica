#include "object.h"

Eng::Object::Object(const std::string& name) : name{ name }
{
}

Eng::Object::~Object()
{
}

const std::string& Eng::Object::getName() const
{
	return name;
}

void Eng::Object::setName(const std::string& name)
{
	this->name = name;
}