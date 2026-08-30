#include "spotLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

Eng::SpotLight::SpotLight(
    const std::string& name,
    const glm::vec3& color)
    : Light{ name, color },
    radius{ 0.0f },
    direction{ 0.0f, 0.0f, -1.0f },
    cutoff{ 180.0f },
    spotExponent{ 0.0f }
{
}

void Eng::SpotLight::setRadius(float radius)
{
    this->radius = radius;
}

float Eng::SpotLight::getRadius() const
{
    return radius;
}

void Eng::SpotLight::setDirection(
    const glm::vec3& direction)
{
    this->direction = direction;
}

const glm::vec3&
Eng::SpotLight::getDirection() const
{
    return direction;
}

void Eng::SpotLight::setCutoff(float cutoff)
{
    this->cutoff = cutoff;
}

float Eng::SpotLight::getCutoff() const
{
    return cutoff;
}

void Eng::SpotLight::setSpotExponent(
    float exponent)
{
    spotExponent = exponent;
}

float Eng::SpotLight::getSpotExponent() const
{
    return spotExponent;
}

void Eng::SpotLight::render(
    const glm::mat4& worldMatrix,
    const glm::mat4& viewMatrix,
    unsigned int lightId)
{
    GLenum id =
        static_cast<GLenum>(lightId);

    glm::vec4 position(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    GLfloat diffuse[] =
    {
        color.r,
        color.g,
        color.b,
        1.0f
    };

    glm::vec3 spotDirection =
        direction;

    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixf(
        glm::value_ptr(
            viewMatrix * worldMatrix
        )
    );

    glLightfv(
        id,
        GL_POSITION,
        glm::value_ptr(position)
    );

    glLightfv(
        id,
        GL_SPOT_DIRECTION,
        glm::value_ptr(spotDirection)
    );

    glLightf(
        id,
        GL_SPOT_CUTOFF,
        cutoff
    );

    glLightf(
        id,
        GL_SPOT_EXPONENT,
        spotExponent
    );

    glLightfv(
        id,
        GL_DIFFUSE,
        diffuse
    );

    glLightfv(
        id,
        GL_SPECULAR,
        diffuse
    );
}