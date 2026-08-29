#include "list.h"
#include "node.h"
#include "mesh.h"
#include "light.h"

Eng::List::List(const std::string& name)
    : Object{ name }
{
}

Eng::List::~List()
{
}

void Eng::List::clear()
{
    elements.clear();
}

const std::list<Eng::RenderElement>& Eng::List::getElements() const
{
    return elements;
}

void Eng::List::pass(Node* root)
{
    clear();

    if (root != nullptr)
        passRecursive(root);
}

void Eng::List::passRecursive(Node* node)
{
    if (node == nullptr)
        return;

    if (dynamic_cast<Light*>(node) != nullptr)
    {
        elements.push_front({
            node,
            node->getWorldMatrix()
        });
    }
    else if (dynamic_cast<Mesh*>(node) != nullptr)
    {
        elements.push_back({
            node,
            node->getWorldMatrix()
        });
    }

    for (Node* child : node->getChildren())
        passRecursive(child);
}