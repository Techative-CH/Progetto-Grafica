/**
 * @file camera.h
 * @brief Camera include file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once

#include "engineApi.h"
#include "node.h"

#include <glm/glm.hpp>

namespace Eng
{
    class ENG_API Camera : public Node
    {
    public:
        Camera(const std::string& name);
        virtual ~Camera();

        void setPerspective(float fov, float aspect, float nearPlane, float farPlane);

        const glm::mat4& getProjectionMatrix() const;
        glm::mat4 getViewMatrix() const;

        void render() override;

    private:
        glm::mat4 projectionMatrix;
    };
}