#include "mygl.h"
#include <glm_includes.h>

#include <QApplication>
#include <QDateTime>
#include <QKeyEvent>
#include <iostream>

#include <QFile>
#include <QImage>
#include <QOpenGLWidget>
#include <iostream>
#include <QJsonArray>
#include <QJsonDocument>

MyGL::MyGL(QWidget* parent)
    : OpenGLContext(parent), m_worldAxes(this), m_progLambert(this),
      m_progPlayer(this), m_progFlat(this), m_terrain(this),
      m_currMSecSinceEpoch(QDateTime::currentMSecsSinceEpoch()), m_time(0.0f),
      m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()),
      m_screenQuad(this), m_progLiquid(this), isInventoryOpen(false), m_player(glm::vec3(48.f, 129.f, 48.f), m_terrain, this) {
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);     // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible

    for (int i = 0; i < 5; i++) {
        Zombie newZombie = Zombie(this);
        m_zombies.push_back(&newZombie);
    }
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL() {
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    m_frameBuffer.create();

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // Create the instance of the world axes
    m_worldAxes.destroyVBOdata();
    m_worldAxes.createVBOdata();

    QJsonObject nodeDataJsonObject = importJson(":/data/nodeData.json");
    m_player.m_geom3D.destroyVBOdata();
    m_player.m_geom3D.createVBOdata();
    m_player.constructSceneGraph(nodeDataJsonObject["PlayerNodes"].toArray());

    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progLiquid.create(":/glsl/liquid.vert.glsl", ":/glsl/liquid.frag.glsl");
    m_progPlayer.create(":/glsl/player.vert.glsl", ":/glsl/player.frag.glsl");

    m_texture = std::make_shared<Texture>(this);
    m_texture->create(":/textures/custom_minecraft_textures.png");
    m_texture->load(0);

    m_playerTexture = std::make_shared<Texture>(this);
    m_playerTexture->create(":/textures/player_texture.png");
    m_playerTexture->load(2);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    m_terrain.CreateTestScene();

    m_screenQuad.destroyVBOdata();
    m_frameBuffer.create();
    m_screenQuad.createVBOdata();
}

void MyGL::resizeGL(int w, int h) {
    // This code sets the concatenated view and perspective projection matrices used for our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera->getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progPlayer.setViewProjMatrix(viewproj);

    // resize framebuffer
    m_frameBuffer.resize(w, h, this->devicePixelRatio());
    m_frameBuffer.destroy();
    m_frameBuffer.create();

    printGLErrorLog();
}

// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    glm::vec3 prevPlayerPos = m_player.m_position;
    float dT = (QDateTime::currentMSecsSinceEpoch() - m_currMSecSinceEpoch) / 1000.f;
    m_player.tick(dT, m_inputs);
    m_currMSecSinceEpoch = QDateTime::currentMSecsSinceEpoch();

    if (m_inputs.underWater) {
        m_progLiquid.setPlayerPosBiomeWts(m_terrain.getBiomeAt(prevPlayerPos.x, prevPlayerPos.z));
        m_progLiquid.setPlayerPos(prevPlayerPos);
        m_progLiquid.setGeometryColor(glm::vec4(0.f, 0.f, 1.f, 1.f));
    } else if (m_inputs.underLava) {
        m_progLiquid.setGeometryColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
    } else {
        m_progLiquid.setGeometryColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
    }

    m_terrain.multithreadedWork(m_player.m_position, prevPlayerPos, dT);
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data

    //m_terrain.loadNewChunks(m_player.mcr_position);
    m_time++;
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.m_position.x, m_player.m_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    m_progLambert.setTime(m_time);

    // Clear the screen so that we only see newly drawn images
    glViewport(0, 0, this->width() * this->devicePixelRatio(),
               this->height() * this->devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_texture->load(0);
    m_texture->bind(0);
    m_progLambert.setTexture(0);

    m_progLambert.setViewProjMatrix(m_player.mcr_camera->getViewProj());
    m_progLambert.setModelMatrix(glm::mat4());

    m_progPlayer.setViewProjMatrix(m_player.mcr_camera->getViewProj());
    m_progPlayer.setModelMatrix(glm::mat4());
    m_progPlayer.setCamPos(m_player.mcr_camera->m_position);

    m_frameBuffer.bindFrameBuffer();
    glViewport(0, 0, this->width() * this->devicePixelRatio(),
               this->height() * this->devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_playerTexture->load(2);
    m_playerTexture->bind(2);
    m_progPlayer.setTexture(2);

    if (m_inputs.inThirdPerson) {
        glDisable(GL_CULL_FACE);
        m_progPlayer.setModelMatrix(glm::mat4());
        m_player.drawSceneGraph(m_player.bodyT, glm::mat4(), m_progPlayer);
        glEnable(GL_CULL_FACE);
    }

    renderTerrain();

    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    m_frameBuffer.bindToTextureSlot(1);
    m_progLiquid.setTexture(m_frameBuffer.getTextureSlot());

    m_progLiquid.draw(m_screenQuad);

    glDisable(GL_DEPTH_TEST);

//    m_progFlat.draw(m_worldAxes);

    glEnable(GL_DEPTH_TEST);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {

    int xFloor = static_cast<int>(glm::floor(m_player.m_position.x / 16.f));
    int zFloor = static_cast<int>(glm::floor(m_player.m_position.z / 16.f));
    int x = 16 * xFloor;
    int z = 16 * zFloor;

    m_terrain.draw(x - 256, x + 256, z - 256, z + 256, &m_progLambert);

    glm::vec2 pPos(m_player.m_position.x, m_player.m_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));

    //m_terrain.getChunkAt(m_player.mcr_position.x, m_player.mcr_position.z).get();



    //m_terrain.draw(pPos.x - 32, pPos.x + 32, pPos.y - 32, pPos.y + 32, &m_progLambert);
}

void MyGL::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }

    if (e->key() == Qt::Key_5) {
        m_player.changeCamera(m_inputs);
    }
    
    if (e->key() == Qt::Key_I) {
        isInventoryOpen = !isInventoryOpen;

        emit sig_sendInventoryToggle(isInventoryOpen);
    }

    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    }

    if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    }

    if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    }

    if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    }

    if (e->key() == Qt::Key_F) {
        m_inputs.flightMode = !m_inputs.flightMode;
    }

    if (m_inputs.flightMode) {
        if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = true;
        }

        if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = true;
        }
    } else {
        if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = true;
        }
    }

    if (QSysInfo().productType() == "macos") {
        float amount = 2.0f;

        if (e->modifiers() & Qt::ShiftModifier) {
            amount = 10.0f;
        }

        if (e->key() == Qt::Key_Right) {
            m_player.rotateOnUpGlobal(3 * -amount);
        }

        if (e->key() == Qt::Key_Left) {
            m_player.rotateOnUpGlobal(3 * amount);
        }

        if (e->key() == Qt::Key_Up) {
            m_player.rotateOnRightLocal(3 * amount);
        }

        if (e->key() == Qt::Key_Down) {
            m_player.rotateOnRightLocal(3 * -amount);
        }
    }
}

void MyGL::keyReleaseEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = false;
    }

    if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = false;
    }

    if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = false;
    }

    if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = false;
    }

    if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = false;
    }

    if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = false;
    }

    if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent* e) {
    if (QSysInfo().productType() != "macos") {
        const float SENSITIVITY = 50.0;
        float dx = this->width() * 0.5 - e->pos().x();

        if (dx != 0) {
            m_player.rotateOnUpGlobal(dx / width() * SENSITIVITY);
        }

        float dy = this->height() * 0.5 - e->pos().y() - 0.5;

        if (dy != 0) {
            m_player.rotateOnRightLocal(dy / height() * SENSITIVITY);
        }

        moveMouseToCenter();
    }
}

void MyGL::mousePressEvent(QMouseEvent *e) {

        if (e->button() == Qt::LeftButton) {
            BlockType removed = m_player.removeBlock(&m_terrain);
        } else if (e->button() == Qt::RightButton) {
            m_player.placeBlock(&m_terrain, currBlock);
        }

}

QJsonObject MyGL::importJson(const char* path) {
    QJsonObject jsonObj;
    QFile file = QFile(path);

    if (file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
        jsonObj = doc.object();

    } else {
        qWarning("Could not open the file.");
    }

    return jsonObj;
}

glm::vec3 MyGL::convertQJsonArrayToGlmVec3(QJsonArray obj) {
    return glm::vec3(obj[0].toDouble(), obj[1].toDouble(), obj[2].toDouble());
}

glm::vec4 MyGL::convertQJsonArrayToGlmVec4(QJsonArray obj) {
    return glm::vec4(obj[0].toDouble(), obj[1].toDouble(), obj[2].toDouble(), obj[3].toDouble());
}
