#pragma once

#include "engineApi.h"
#include "node.h"

#include <glm/glm.hpp>

namespace Eng
{
    class ENG_API Light : public Node
    {
    public:
        enum class Subtype
        {
            OMNI = 0,
            DIRECTIONAL,
            SPOT
        };

        Light(const std::string& name);
        virtual ~Light();

        void setSubtype(Subtype subtype);
        Subtype getSubtype() const;

        void setColor(float r, float g, float b);
        const glm::vec3& getColor() const;

        void setRadius(float radius);
        float getRadius() const;

        void setDirection(const glm::vec3& direction);
        const glm::vec3& getDirection() const;

        void setCutoff(float cutoff);
        float getCutoff() const;

        void setSpotExponent(float exponent);
        float getSpotExponent() const;

        void setCastShadows(bool value);
        bool getCastShadows() const;

        void setVolumetric(bool value);
        bool getVolumetric() const;

        void render() override;

    private:
        Subtype subtype;
        glm::vec3 color;

        float radius;
        glm::vec3 direction;
        float cutoff;
        float spotExponent;

        bool castShadows;
        bool volumetric;
    };
}