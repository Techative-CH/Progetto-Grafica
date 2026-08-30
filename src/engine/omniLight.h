#pragma once

#include "light.h"

namespace Eng
{
    class ENG_API OmniLight : public Light
    {
    public:
        OmniLight(
            const std::string& name,
            const glm::vec3& color
        );

        void setRadius(float radius);
        float getRadius() const;

        void render(
            const glm::mat4& worldMatrix,
            const glm::mat4& viewMatrix,
            unsigned int lightId
        ) override;

    private:
        float radius;
    };
}