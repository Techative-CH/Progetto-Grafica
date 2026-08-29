#include "material.h"
#include "texture.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

Eng::Material::Material(const std::string& name)
    : Object{ name },
    texture{ nullptr },
    emission{ 0.0f, 0.0f, 0.0f, 1.0f },
    ambient{ 0.2f, 0.2f, 0.2f, 1.0f },
    diffuse{ 0.8f, 0.8f, 0.8f, 1.0f },
    specular{ 0.0f, 0.0f, 0.0f, 1.0f },
    shininess{ 0.0f }
{
}

Eng::Material::~Material()
{
}

void Eng::Material::setTexture(Texture* texture)
{
    this->texture = texture;
}

Eng::Texture* Eng::Material::getTexture() const
{
    return texture;
}

void Eng::Material::setEmission(float r, float g, float b, float a)
{
    emission = glm::vec4(r, g, b, a);
}

void Eng::Material::setAmbient(float r, float g, float b, float a)
{
    ambient = glm::vec4(r, g, b, a);
}

void Eng::Material::setDiffuse(float r, float g, float b, float a)
{
    diffuse = glm::vec4(r, g, b, a);
}

void Eng::Material::setSpecular(float r, float g, float b, float a)
{
    specular = glm::vec4(r, g, b, a);
}

void Eng::Material::setShininess(float value)
{
    shininess = value;
}

const glm::vec4& Eng::Material::getEmission() const
{
    return emission;
}

const glm::vec4& Eng::Material::getAmbient() const
{
    return ambient;
}

const glm::vec4& Eng::Material::getDiffuse() const
{
    return diffuse;
}

const glm::vec4& Eng::Material::getSpecular() const
{
    return specular;
}

float Eng::Material::getShininess() const
{
    return shininess;
}

void Eng::Material::render()
{
    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_EMISSION,
        glm::value_ptr(emission)
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_AMBIENT,
        glm::value_ptr(ambient)
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_DIFFUSE,
        glm::value_ptr(diffuse)
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_SPECULAR,
        glm::value_ptr(specular)
    );

    glMaterialf(
        GL_FRONT_AND_BACK,
        GL_SHININESS,
        shininess
    );

    if (texture != nullptr)
        texture->bind();
}

void Eng::Material::unbind()
{
    if (texture != nullptr)
        texture->unbind();
}