#pragma once
#include <QJsonObject>
#include "geometry3d.h"
#include "scene/chunk.h"
#include "scene/node.h"
#include "shaderprogram.h"

struct InputBundle {
    bool wPressed, aPressed, sPressed, dPressed, ePressed, fPressed, qPressed, spacePressed;
    float mouseX, mouseY;
    bool flightMode, onGround, inLiquid, underWater, underLava, isMoving;
    glm::vec3 playerPosition;
    bool collisionDetected, isZombie, isPig, isHoldingWheat;

    InputBundle()
        : wPressed(false), aPressed(false), sPressed(false),
          dPressed(false), ePressed(false), fPressed(false),
          qPressed(false), spacePressed(false),
          mouseX(0.f), mouseY(0.f), flightMode(true), onGround(false), inLiquid(false),
          underWater(false), underLava(false), isMoving(false),
          isZombie(false), isPig(false), isHoldingWheat(false)
    {}
};

class MyGL;
class Terrain;

class Entity {
protected:
    // Store reference to game context and terrain for easy access.
    // Make class reference variables when there is no need to make null or reassign
    MyGL& mr_context;
    Terrain& mr_terrain;

    static const inline std::vector<float> HUMANOID_DIMENSIONS = { 0.125f, 0.875f, 0.25f, 0.75f, 0.f, 1.9f };

    void isInLiquid();
    void isOnGround();
    void isUnderLiquid();
public:
    InputBundle m_inputs;
    int infAxis;
    glm::vec3 m_velocity, m_acceleration;
    glm::vec3 m_forward, m_right, m_up;
    glm::vec3 m_position;
    Geometry3D m_geom3D;
    uPtr<Node> mp_rootNode;
    std::unordered_map<QString, Node*> mp_nodePointerMap;

    float m_timer;

    Entity(MyGL& context, Terrain& terrain, std::optional<glm::vec3> pos = glm::vec3(0, 0, 0));

    // To be called by MyGL::tick()
    virtual void tick(float dT) = 0;

    virtual void computePhysics(float dT);
    virtual void detectCollision();
    virtual bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, float* out_dist, glm::ivec3* out_blockHit, BlockType* out_type = nullptr);

    // Translate along the given vector
    virtual void moveAlongVector(glm::vec3 dir);

    // Translate along one of our local axes
    virtual void moveForwardLocal(float amount);
    virtual void moveRightLocal(float amount);
    virtual void moveUpLocal(float amount);

    // Translate along one of the world axes
    virtual void moveForwardGlobal(float amount);
    virtual void moveRightGlobal(float amount);
    virtual void moveUpGlobal(float amount);

    // Rotate about one of our local axes
    virtual void rotateOnForwardLocal(float degrees);
    virtual void rotateOnRightLocal(float degrees);
    virtual void rotateOnUpLocal(float degrees);

    // Rotate about one of the world axes
    virtual void rotateOnForwardGlobal(float degrees);
    virtual void rotateOnRightGlobal(float degrees);
    virtual void rotateOnUpGlobal(float degrees);

    // TODO: create humanoid subclass
    void constructSceneGraph(QJsonArray data);
    void drawSceneGraph(const uPtr<Node>& currNode, glm::mat4 currTransformation, ShaderProgram& m_progLambert);
    virtual void animate(float dT);
};
