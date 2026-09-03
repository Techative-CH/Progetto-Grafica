#pragma once

#include "light.h"

namespace Eng
{
    class ENG_API DirectionalLight : public Light
    {
    public:
        DirectionalLight(const std::string& name, const glm::vec3& color);

        void setDirection(const glm::vec3& direction);
        const glm::vec3& getDirection() const;

        void renderLight(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, unsigned int lightId) override;

    private:
        glm::vec3 direction;
    };
}
