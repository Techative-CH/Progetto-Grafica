/**
 * @file material.h
 * @brief Material include file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once

#include "engineApi.h"
#include "object.h"

#include <glm/glm.hpp>

namespace Eng
{
    class Texture;

    class ENG_API Material : public Object
    {
    public:
        Material(const std::string& name);
        virtual ~Material();

        void setTexture(Texture* texture);
        Texture* getTexture() const;

        void setEmission(float r, float g, float b, float a = 1.0f);
        void setAmbient(float r, float g, float b, float a = 1.0f);
        void setDiffuse(float r, float g, float b, float a = 1.0f);
        void setSpecular(float r, float g, float b, float a = 1.0f);
        void setShininess(float value);

        const glm::vec4& getEmission() const;
        const glm::vec4& getAmbient() const;
        const glm::vec4& getDiffuse() const;
        const glm::vec4& getSpecular() const;
        float getShininess() const;

        void render();
        void unbind();

    private:
        Texture* texture;

        glm::vec4 emission;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;

        float shininess;
    };
}