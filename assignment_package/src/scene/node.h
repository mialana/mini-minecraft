#pragma once
#include "smartpointerhelp.h"
#include "geometry3d.h"

class Node {
protected:
    void copyChildren(const Node &n2);
    std::vector<uPtr<Node>> children;
public:
    Geometry3D* geometry;
    QString geomType;

    Node(Geometry3D*);
    Node(const Node&);
    virtual ~Node();
    Node& operator=(const Node&);

    virtual glm::mat4 transformMatrix() = 0;

    Node& addChild(uPtr<Node>);

    const std::vector<uPtr<Node>>& getChildren();
};

class TranslateNode : public Node {
public:
    glm::vec3 translation;

    TranslateNode(Geometry3D*, glm::vec3);
    TranslateNode(const TranslateNode&);
    virtual ~TranslateNode();
    TranslateNode& operator=(const TranslateNode&);

    glm::mat4 transformMatrix() override;
};

class RotateNode : public Node {
public:
    float degrees;
    glm::vec3 axisOfRotation;

    RotateNode(Geometry3D*);
    RotateNode(Geometry3D*, float, glm::vec3);
    RotateNode(const RotateNode&);
    virtual ~RotateNode();
    RotateNode& operator=(const RotateNode&);

    glm::mat4 transformMatrix() override;
};

class ScaleNode : public Node {
public:
    glm::vec3 scale;

    ScaleNode(Geometry3D*, glm::vec3);
    ScaleNode(const ScaleNode&);
    virtual ~ScaleNode();
    ScaleNode& operator=(const ScaleNode&);

    glm::mat4 transformMatrix() override;
};

