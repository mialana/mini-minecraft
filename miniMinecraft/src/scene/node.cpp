#include "node.h"
#include "math.h"

Node::Node(Geometry3D* g)
    : geometry(g) {};

void Node::copyChildren(const Node& n2)
{
    for (const uPtr<Node>& child : n2.children) {
        TranslateNode* t = dynamic_cast<TranslateNode*>(child.get());

        if (t != nullptr) {
            this->children.push_back(mkU<TranslateNode>(*t));
        } else {
            RotateNode* r = dynamic_cast<RotateNode*>(child.get());

            if (r != nullptr) {
                this->children.push_back(mkU<RotateNode>(*r));
            } else {
                ScaleNode* s = dynamic_cast<ScaleNode*>(child.get());

                if (s != nullptr) {
                    this->children.push_back(mkU<ScaleNode>(*s));
                }
            }
        }
    }
}

Node::Node(const Node& n2)
    : Node(n2.geometry)
{
    this->copyChildren(n2);
}

Node::~Node() {};

Node& Node::operator=(const Node& n2)
{
    geometry = n2.geometry;

    this->children.clear();
    this->copyChildren(n2);
    return *this;
}

Node& Node::addChild(uPtr<Node> n)
{
    this->children.push_back(std::move(n));
    return *(children.back());
}

const std::vector<uPtr<Node>>& Node::getChildren()
{
    return children;
}

/* TranslateNode Implementation */

TranslateNode::TranslateNode(Geometry3D* g, glm::vec3 t)
    : Node(g)
    , translation(t)
{}

TranslateNode::TranslateNode(const TranslateNode& n2)
    : Node(n2)
    , translation(n2.translation) {};

TranslateNode::~TranslateNode() {};

TranslateNode& TranslateNode::operator=(const TranslateNode& n2)
{
    Node::operator=(n2);
    translation = n2.translation;
    return *this;
}

glm::mat4 TranslateNode::transformMatrix()
{
    glm::vec4 col1 = glm::vec4(1, 0, 0, 0);
    glm::vec4 col2 = glm::vec4(0, 1, 0, 0);
    glm::vec4 col3 = glm::vec4(0, 0, 1, 0);
    glm::vec4 col4 = glm::vec4(translation.x, translation.y, translation.z, 1.f);
    return glm::mat4(col1, col2, col3, col4);
}

/* RotateNode Implementation */

RotateNode::RotateNode(Geometry3D* g)
    : Node(g)
    , degrees(0)
    , axisOfRotation(glm::vec3(1, 0, 0))
{}

RotateNode::RotateNode(Geometry3D* g, float d, glm::vec3 aor)
    : Node(g)
    , degrees(d)
    , axisOfRotation(aor)
{}

RotateNode::RotateNode(const RotateNode& n2)
    : Node(n2)
    , degrees(n2.degrees)
    , axisOfRotation(n2.axisOfRotation) {};

RotateNode::~RotateNode() {};

RotateNode& RotateNode::operator=(const RotateNode& n2)
{
    Node::operator=(n2);
    axisOfRotation = n2.axisOfRotation;
    return *this;
}

glm::mat4 RotateNode::transformMatrix()
{
    return glm::rotate(glm::mat4(), glm::radians(degrees), axisOfRotation);
}

/* ScaleNode Implementation */

ScaleNode::ScaleNode(Geometry3D* g, glm::vec3 s)
    : Node(g)
    , scale(s)
{}

ScaleNode::ScaleNode(const ScaleNode& n2)
    : Node(n2)
    , scale(n2.scale) {};

ScaleNode& ScaleNode::operator=(const ScaleNode& n2)
{
    Node::operator=(n2);
    scale = n2.scale;
    return *this;
}

ScaleNode::~ScaleNode() {};

glm::mat4 ScaleNode::transformMatrix()
{
    glm::vec4 col1 = glm::vec4(scale.x, 0, 0, 0);
    glm::vec4 col2 = glm::vec4(0, scale.y, 0, 0);
    glm::vec4 col3 = glm::vec4(0, 0, scale.z, 0);
    glm::vec4 col4 = glm::vec4(0, 0, 0, 1);
    return glm::mat4(col1, col2, col3, col4);
}
