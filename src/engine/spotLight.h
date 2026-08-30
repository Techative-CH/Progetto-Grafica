#pragma once

#include "light.h"

namespace Eng
{
    class ENG_API SpotLight : public Light
    {
    public:
        SpotLight(
            const std::string& name,
            const glm::vec3& color
        );

        void setRadius(float radius);
        float getRadius() const;

        void setDirection(
            const glm::vec3& direction
        );

        const glm::vec3& getDirection() const;

        void setCutoff(float cutoff);
        float getCutoff() const;

        void setSpotExponent(float exponent);
        float getSpotExponent() const;

        void renderLight(
            const glm::mat4& worldMatrix,
            const glm::mat4& viewMatrix,
            unsigned int lightId
        ) override;

    private:
        float radius;

        glm::vec3 direction;

        float cutoff;
        float spotExponent;
    };
}
