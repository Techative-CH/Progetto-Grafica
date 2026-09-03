#include "material.h"
#include "texture.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>


/**
 * @brief Creates a material with default OpenGL material properties.
 * @param name Name of the material
 */
Eng::Material::Material(const std::string& name)
    : Object{ name },
    texture{ nullptr },
    emission{ 0.0f, 0.0f, 0.0f, 1.0f },
    ambient{ 0.2f, 0.2f, 0.2f, 1.0f },
    diffuse{ 0.8f, 0.8f, 0.8f, 1.0f },
    specular{ 0.0f, 0.0f, 0.0f, 1.0f },
    shininess{ 0.0f }
{}

/**
 * @brief Destroys the material.
 */
Eng::Material::~Material()
{}

/**
 * @brief Assigns a texture to the material.
 * @param texture Texture to assign
 */
void Eng::Material::setTexture(Texture* texture)
{
    this->texture = texture;
}

/**
 * @brief Returns the texture assigned to the material.
 * @return Pointer to the assigned texture
 */
Eng::Texture* Eng::Material::getTexture() const
{
    return texture;
}

/**
 * @brief Sets the emission color of the material.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 */
void Eng::Material::setEmission(float r, float g, float b, float a)
{
    emission = glm::vec4(r, g, b, a);
}

/**
 * @brief Sets the ambient color of the material.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 */
void Eng::Material::setAmbient(float r, float g, float b, float a)
{
    ambient = glm::vec4(r, g, b, a);
}

/**
 * @brief Sets the diffuse color of the material.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 */
void Eng::Material::setDiffuse(float r, float g, float b, float a)
{
    diffuse = glm::vec4(r, g, b, a);
}

/**
 * @brief Sets the specular color of the material.
 * @param r Red component
 * @param g Green component
 * @param b Blue component
 * @param a Alpha component
 */
void Eng::Material::setSpecular(float r, float g, float b, float a)
{
    specular = glm::vec4(r, g, b, a);
}

/**
 * @brief Sets the shininess coefficient of the material.
 * @param value Shininess coefficient
 */
void Eng::Material::setShininess(float value)
{
    shininess = value;
}

/**
 * @brief Returns the emission color of the material.
 * @return The emission color
 */
const glm::vec4& Eng::Material::getEmission() const
{
    return emission;
}

/**
 * @brief Returns the ambient color of the material.
 * @return The ambient color
 */
const glm::vec4& Eng::Material::getAmbient() const
{
    return ambient;
}

/**
 * @brief Returns the diffuse color of the material.
 * @return The diffuse color
 */
const glm::vec4& Eng::Material::getDiffuse() const
{
    return diffuse;
}

/**
 * @brief Returns the specular color of the material.
 * @return The specular color
 */
const glm::vec4& Eng::Material::getSpecular() const
{
    return specular;
}

/**
 * @brief Returns the shininess coefficient of the material.
 * @return The shininess coefficient
 */
float Eng::Material::getShininess() const
{
    return shininess;
}

/**
 * @brief Applies the material properties and its optional texture to OpenGL.
 */
void Eng::Material::render()
{
    // Enable and bind the texture only if the material has one
    if (texture != nullptr)
    {
        glEnable(GL_TEXTURE_2D);
        texture->render();
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(emission)); // Emitted light
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(ambient)); // Ambient light reflection
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(diffuse)); // Diffuse light reflection
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(specular)); // Specular light reflection
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // Specular highlight size
}