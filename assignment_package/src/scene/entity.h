#pragma once
#include "glm_includes.h"
#include <QJsonObject>
#include "geometry3d.h"
#include "scene/node.h"
#include "shaderprogram.h"

struct InputBundle {
    bool wPressed, aPressed, sPressed, dPressed, ePressed, fPressed, qPressed,
         spacePressed;
    float mouseX, mouseY;
    bool flightMode, onGround, inLiquid, underWater, underLava, inThirdPerson, isMoving;

    InputBundle()
        : wPressed(false), aPressed(false), sPressed(false),
          dPressed(false), ePressed(false), fPressed(false),
          qPressed(false), spacePressed(false),
          mouseX(0.f), mouseY(0.f), flightMode(true), onGround(false), isMoving(false),
          inLiquid(false), underWater(false), underLava(false), inThirdPerson(false)
    {}
};

class Entity {
    public:
        glm::vec3 m_forward, m_right, m_up;
        glm::vec3 m_position;
        Geometry3D m_geom3D;

        // Various constructors
        Entity(OpenGLContext* context);
        Entity(glm::vec3 pos, OpenGLContext* context);
        Entity(const Entity& e, OpenGLContext* context);
        virtual ~Entity();

        // To be called by MyGL::tick()
        virtual void tick(float dT, InputBundle& input) = 0;

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

        virtual void constructSceneGraph(QJsonArray);
        void drawSceneGraph(const uPtr<Node>& currNode, glm::mat4 currTransformation,
                            ShaderProgram& m_progLambert);
        virtual void animate(float dT, InputBundle& inputs);

        float m_timer;
};
