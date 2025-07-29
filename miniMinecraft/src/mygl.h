#ifndef MYGL_H
#define MYGL_H

#include "openglcontext.h"
#include "scene/quad.h"
#include "scene/mob.h"
#include "shaderprogram.h"
#include "scene/worldaxes.h"
#include "scene/terrain.h"
#include "scene/player.h"
#include "framebuffer.h"
#include "texture.h"

#include <memory>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>

class MyGL : public OpenGLContext
{
    Q_OBJECT

private:
    WorldAxes
        m_worldAxes;  // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    ShaderProgram m_progLambert;  // A shader program that uses lambertian reflection
    ShaderProgram m_progPlayer;
    ShaderProgram m_progFlat;

    GLuint vao;  // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
    // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Terrain m_terrain;  // All of the Chunks that currently comprise the world.

    qint64 m_currMSecSinceEpoch;

    QTimer m_timer;            // Timer linked to tick(). Fires approximately 60 times per second.

    void moveMouseToCenter();  // Forces the mouse position to the screen's center. You should call this
    // from within a mouse move event after reading the mouse movement so that
    // your mouse stays within the screen bounds and is always read.

    void sendPlayerDataToGUI() const;

    int m_time;

    // for post-process shading when in water or lava
    FrameBuffer m_frameBuffer;
    Quad m_screenQuad;
    ShaderProgram m_progLiquid;

    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Texture> m_playerTexture;
    std::shared_ptr<Texture> m_pigTexture;
    std::shared_ptr<Texture> m_zombieTexture;

public:
    bool isInventoryOpen;
    Player m_player;

    std::vector<uPtr<Mob>> m_mobs;

    BlockType currBlock = EMPTY;

    explicit MyGL(QWidget* parent = nullptr);
    ~MyGL();

    // Called once when MyGL is initialized.
    // Once this is called, all OpenGL function
    // invocations are valid (before this, they
    // will cause segfaults)
    void initializeGL() override;
    // Called whenever MyGL is resized.
    void resizeGL(int w, int h) override;
    // Called whenever MyGL::update() is called.
    // In the base code, update() is called from tick().
    void paintGL() override;

    // Called from paintGL().
    // Calls Terrain::draw().
    void renderTerrain();

    static QJsonObject importJson(const char* path);
    static glm::vec3 convertQJsonArrayToGlmVec3(QJsonArray obj);
    static glm::vec4 convertQJsonArrayToGlmVec4(QJsonArray obj);

    void showRecipe();

protected:
    // Automatically invoked when the user
    // presses a key on the keyboard
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    // Automatically invoked when the user
    // moves the mouse
    void mouseMoveEvent(QMouseEvent* e);
    // Automatically invoked when the user
    // presses a mouse button
    void mousePressEvent(QMouseEvent* e);

private slots:
    void tick();  // Slot that gets called ~60 times per second by m_timer firing.

signals:
    void sig_sendPlayerPos(QString) const;
    void sig_sendPlayerVel(QString) const;
    void sig_sendPlayerAcc(QString) const;
    void sig_sendPlayerLook(QString) const;
    void sig_sendPlayerChunk(QString) const;
    void sig_sendPlayerTerrainZone(QString) const;

    void sig_sendInventoryToggle(bool) const;

    void sig_sendRecipeWindow() const;
};

#endif  // MYGL_H
