#include "object.h"

/**
 * @brief Creates an object with the specified name.
 * @param name Name of the object
 */
Eng::Object::Object(const std::string& name) : 
	name{ name }
{}

/**
 * @brief Destroys the object.
 */
Eng::Object::~Object()
{}

/**
 * @brief Returns the name of the object.
 * @return The object name
 */
const std::string& Eng::Object::getName() const
{
	return name;
}

/**
 * @brief Sets the name of the object.
 * @param name The new name of the object
 */
void Eng::Object::setName(const std::string& name)
{
	this->name = name;
}