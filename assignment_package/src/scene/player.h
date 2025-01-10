#pragma once
#include "entity.h"
#include "camera.h"
#include "scene/node.h"
#include "InventoryManager.h"

enum CameraViews : unsigned char {
    FIRST, SECOND, THIRD
};

const static std::vector<float> playerDimensions = {
    0.125f, 0.875f, 0.25f, 0.75f, 0.f, 1.9f
};

class Player : public Entity {
    private:
        Camera m_firstPersonCamera;
        Camera m_secondPersonCamera;
        Camera m_thirdPersonCamera;

        void processInputs();

        void calculateThirdPersonCameraRotation();
        void calculateSecondPersonCameraRotation();
    public:
        CameraViews m_activeCameraView;

        CameraViews changeActiveCamera();
        Camera const& getActiveCamera();

        InventoryManager inventory;

        Player(OpenGLContext& context, Terrain& terrain, glm::vec3 pos);

        void setCameraWidthHeight(unsigned int w, unsigned int h);

        void tick(float dT, Terrain& terrain) override;

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
