#pragma once

#include "Transform2D.hpp"

#include <vector>
#include <functional>

class Node
{
public:
    Transform2D relative;
    Transform2D absolute;

    Node* parent = nullptr;
    std::vector<Node*> children;
    bool isDirty = true;

    void addChild(Node* child);
    void removeChild(Node* child);
    void updatePosition();
    void setRelative(const Transform2D& relative);

    using UpdateMethod = std::function<bool(Node&)>;
    void addUpdateMethod(const UpdateMethod& method);
    bool update();
    std::vector<UpdateMethod> updateMethods;
};
