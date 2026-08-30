#include "directionalLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

Eng::DirectionalLight::DirectionalLight(
    const std::string& name,
    const glm::vec3& color)
    : Light{ name, color },
    direction{ 0.0f, 0.0f, -1.0f }
{
}

void Eng::DirectionalLight::setDirection(
    const glm::vec3& direction)
{
    this->direction = direction;
}

const glm::vec3&
Eng::DirectionalLight::getDirection() const
{
    return direction;
}

void Eng::DirectionalLight::renderLight(
    const glm::mat4& worldMatrix,
    const glm::mat4& viewMatrix,
    unsigned int lightId)
{
    GLenum id =
        static_cast<GLenum>(lightId);

    glm::vec4 position(
        -direction.x,
        -direction.y,
        -direction.z,
        0.0f
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
