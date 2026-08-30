#pragma once

#include "node.h"

#include <glm/glm.hpp>

namespace Eng
{
    class ENG_API Light : public Node
    {
    public:
        Light(
            const std::string& name,
            const glm::vec3& color
        );

        virtual ~Light();

        void render() override {}

        virtual void renderLight(
            const glm::mat4& worldMatrix,
            const glm::mat4& viewMatrix,
            unsigned int lightId
        ) = 0;

        void setColor(float r, float g, float b);
        const glm::vec3& getColor() const;

        void setCastShadows(bool value);
        bool getCastShadows() const;

        void setVolumetric(bool value);
        bool getVolumetric() const;

    protected:
        glm::vec3 color;

        bool castShadows;
        bool volumetric;
    };
}
