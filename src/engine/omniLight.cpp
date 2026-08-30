#include "omniLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

Eng::OmniLight::OmniLight(
    const std::string& name,
    const glm::vec3& color)
    : Light{ name, color },
    radius{ 0.0f }
{
}

void Eng::OmniLight::setRadius(float radius)
{
    this->radius = radius;
}

float Eng::OmniLight::getRadius() const
{
    return radius;
}

void Eng::OmniLight::render(
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
        GL_DIFFUSE,
        diffuse
    );

    glLightfv(
        id,
        GL_SPECULAR,
        diffuse
    );

    glLightf(
        id,
        GL_SPOT_CUTOFF,
        180.0f
    );
}