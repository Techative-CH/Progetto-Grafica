#include "list.h"
#include "node.h"
#include "mesh.h"
#include "light.h"

/**
 * @brief Constructs a render list with the specified name.
 * @param name Name of the list
 */
Eng::List::List(const std::string& name)
    : Object{ name }
{}

/**
 * @brief Destroys the render list.
 */
Eng::List::~List()
{}

/**
 * @brief Removes all elements from the list
 */
void Eng::List::clear()
{
    elements.clear();
}

/**
 * @brief Returns all elements currently stored in the render list.
 * @return Reference to the list of render elements
 */
const std::list<Eng::RenderElement>& Eng::List::getElements() const
{
    return elements;
}

/**
 * @brief Builds the render list by traversing the scene graph.
 * @param root Root node from which the traversal starts
 */
void Eng::List::pass(Node* root)
{
    clear();

    if (root != nullptr)
        passRecursive(root);
}

/**
 * @brief Recursively traverses the scene graph.
 * @param node Current node being processed
 */
void Eng::List::passRecursive(Node* node)
{
    if (node == nullptr)
        return;

    // Lights inserted at the beginning to process them before meshes
    if (dynamic_cast<Light*>(node) != nullptr)
    {
        elements.push_front({
            node,
            node->getWorldMatrix()
        });
    }

    // Meshes inserted at the end of list
    else if (dynamic_cast<Mesh*>(node) != nullptr)
    {
        elements.push_back({
            node,
            node->getWorldMatrix()
            });
    }

    // Recursively process all children of the current node
    for (Node* child : node->getChildren())
        passRecursive(child);
}