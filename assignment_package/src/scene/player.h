#pragma once
#include "entity.h"
#include "camera.h"
#include "scene/node.h"
#include "terrain.h"
#include "InventoryManager.h"

const static std::vector<float> playerDimensions = {
    0.125f, 0.875f, 0.25f, 0.75f, 0.f, 1.9f
};

class Player : public Entity {
    private:
        Camera m_camera;
        Camera m_thirdPersonCamera;
        Camera m_frontViewCamera;
        const Terrain& mcr_terrain;

        int infAxis;

        void processInputs(InputBundle& inputs);
        void computePhysics(float dT, InputBundle& inputs);

    public:
        void calculateThirdPersonCameraRotation();
        void calculateFrontViewCameraRotation();
        void changeCamera(InputBundle& inputs);
        InventoryManager inventory;

        Player(glm::vec3 pos, const Terrain &terrain);
        virtual ~Player() override;

        // Readonly public reference to our camera
        // for easy access from MyGL
        Camera* mcr_camera;

        Player(glm::vec3 pos, const Terrain& terrain, OpenGLContext* context);

        void setCameraWidthHeight(unsigned int w, unsigned int h);

        void tick(float dT, InputBundle& inputs) override;

        void isInLiquid(InputBundle& input);
        void isUnderLiquid(InputBundle& input);
        void isOnGround(InputBundle& input);
        void detectCollision();

        bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, float* out_dist, glm::ivec3* out_blockHit);

        BlockType placeBlock(Terrain* terrain, BlockType currBlock);
        BlockType removeBlock(Terrain* terrain);

        // Player overrides all of Entity's movement
        // functions so that it transforms its camera
        // by the same amount as it transforms itself.
        void moveAlongVector(glm::vec3 dir) override;
        void moveForwardLocal(float amount) override;
        void moveRightLocal(float amount) override;
        void moveUpLocal(float amount) override;
        void moveForwardGlobal(float amount) override;
        void moveRightGlobal(float amount) override;
        void moveUpGlobal(float amount) override;
        void rotateOnForwardLocal(float degrees) override;
        void rotateOnRightLocal(float degrees) override;
        void rotateOnUpLocal(float degrees) override;
        void rotateOnForwardGlobal(float degrees) override;
        void rotateOnRightGlobal(float degrees) override;
        void rotateOnUpGlobal(float degrees) override;

        // For sending the Player's data to the GUI
        // for display
        QString posAsQString() const;
        QString velAsQString() const;
        QString accAsQString() const;
        QString lookAsQString() const;
};
