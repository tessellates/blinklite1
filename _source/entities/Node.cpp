#include "Node.hpp"

void Node::addChild(Node* child) 
{
    if (child->parent) {
        child->parent->removeChild(child);
    }
    children.push_back(child);
    child->parent = this;
    child->absolute = calculateAbsoluteTransform(child->relative, absolute);
}

void Node::removeChild(Node* child) 
{
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
    child->parent = nullptr;
}

void Node::updatePosition()
{
    if (parent)
    {
        if (parent->isDirty)
        {
            absolute = calculateAbsoluteTransform(relative, parent->absolute);
        }
    }
    else
    {
        absolute = relative;
    }
}

void Node::setRelative(const Transform2D& relativet)
{
    relative = relativet;
    this->isDirty = true;
}

void Node::addUpdateMethod(const UpdateMethod& method)
{
    updateMethods.push_back(method);
}

bool Node::update()
{
    bool updated = false;
    for (auto& method : updateMethods)
    {
        if (method(*this)) 
        {
            updated = true;
        }
    }
    if (updated)
        isDirty = true;

    return updated;
}
