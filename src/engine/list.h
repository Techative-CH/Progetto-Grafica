/**
 * @file list.h
 * @brief List include file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#pragma once

#include "engineApi.h"
#include "object.h"
#include "node.h"

#include <list>
#include <glm/glm.hpp>

namespace Eng
{
    struct RenderElement
    {
        Node* node;
        glm::mat4 worldMatrix;
    };

    class ENG_API List : public Object
    {
    public:
        List(const std::string& name);
        virtual ~List();

        void clear();
        void pass(Node* root);

        const std::list<RenderElement>& getElements() const;

    private:
        std::list<RenderElement> elements;

        void passRecursive(Node* node);
    };
}