#pragma once
#include "geometry3d.h"
#include "scene/chunk.h"
#include "scene/node.h"
#include "shaderprogram.h"
#include <QJsonObject>
#include "patharrow.h"

struct InputBundle
{
    bool wPressed, aPressed, sPressed, dPressed, ePressed, fPressed, qPressed, spacePressed;
    float mouseX, mouseY;
    bool flightMode, onGround, inLiquid, underWater, underLava, inThirdPerson, isMoving;
    glm::vec3 playerPosition;
    bool collisionDetected, isZombie, isPig, isHoldingWheat;

    InputBundle()
        : wPressed(false)
        , aPressed(false)
        , sPressed(false)
        , dPressed(false)
        , ePressed(false)
        , fPressed(false)
        , qPressed(false)
        , spacePressed(false)
        , mouseX(0.f)
        , mouseY(0.f)
        , flightMode(true)
        , onGround(false)
        , inLiquid(false)
        , underWater(false)
        , underLava(false)
        , inThirdPerson(false)
        , isMoving(false)
        , isZombie(false)
        , isPig(false)
        , isHoldingWheat(false)
    {}
};

class Terrain;

class Entity
{
protected:
    void isInLiquid(Terrain& terrain);
    void isOnGround(Terrain& terrain);
    void isUnderLiquid(Terrain& terrain);

public:
    float m_timer;

    InputBundle m_inputs;
    int infAxis;
    glm::vec3 m_velocity, m_acceleration;
    glm::vec3 m_forward, m_right, m_up;
    glm::vec3 m_position;
    Geometry3D m_geom3D;
    uPtr<Node> bodyT;
    std::unordered_map<QString, Node*> nodePointerMap;

    // Various constructors
    Entity(OpenGLContext* context);
    Entity(glm::vec3 pos, OpenGLContext* context);
    Entity(const Entity& e, OpenGLContext* context);
    virtual ~Entity();

    // To be called by MyGL::tick()
    virtual void tick(float dT, Terrain& terrain) = 0;

    virtual void computePhysics(float dT, Terrain& terrain);
    virtual void detectCollision(Terrain& terrain);
    virtual bool gridMarch(glm::vec3 rayOrigin,
                           glm::vec3 rayDirection,
                           float* out_dist,
                           glm::ivec3* out_blockHit,
                           Terrain& terrain,
                           BlockType* out_type = nullptr);

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

    void constructSceneGraph(QJsonArray data);
    void drawSceneGraph(const uPtr<Node>& currNode,
                        glm::mat4 currTransformation,
                        ShaderProgram& progShader,
                        ShaderProgram& progFlat);
    virtual void animate(float dT);

    // currently unused
    void drawPath(Terrain& terrain);
};
