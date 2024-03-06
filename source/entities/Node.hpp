#pragma once

#include "Transform2D.hpp"

#include <vector>

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
    //void update
};
