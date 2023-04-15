#pragma once
#include <openglcontext.h>
#include <glm_includes.h>

//This defines a class which can be rendered by our shader program.
//Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
class Drawable
{
protected:
    int m_oCount;     // The number of indices stored in bufIdx.

    GLuint m_oBufIdx; // A Vertex Buffer Object that we will use to store triangle indices (GLuints)
    GLuint m_oBufPos; // A Vertex Buffer Object that we will use to store mesh vertices (vec4s)
    GLuint m_oBufNor; // A Vertex Buffer Object that we will use to store mesh normals (vec4s)
    GLuint m_oBufCol; // Can be used to pass per-vertex color information to the shader, but is currently unused.
                   // Instead, we use a uniform vec4 in the shader to set an overall color for the geometry
    GLuint m_oBufUVs;
    GLuint m_oBufBTs;
    GLuint m_oBufBWts;
    GLuint m_oBufVertData;

    bool m_oIdxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool m_oPosGenerated;
    bool m_oNorGenerated;
    bool m_oColGenerated;
    bool m_oUVsGenerated;
    bool m_oBTsGenerated;
    bool m_oBWtsGenerated;
    bool m_oVertDataGenerated;

    int m_tCount;

    GLuint m_tBufIdx;
    GLuint m_tBufPos;
    GLuint m_tBufNor;
    GLuint m_tBufCol;
    GLuint m_tBufUVs;
    GLuint m_tBufBTs;
    GLuint m_tBufBWts;
    GLuint m_tBufVertData;

    bool m_tIdxGenerated; // Set to TRUE by generateIdx(), returned by bindIdx().
    bool m_tPosGenerated;
    bool m_tNorGenerated;
    bool m_tColGenerated;
    bool m_tUVsGenerated;
    bool m_tBTsGenerated;
    bool m_tBWtsGenerated;
    bool m_tVertDataGenerated;

    OpenGLContext* mp_context; // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                          // we need to pass our OpenGL context to the Drawable in order to call GL functions
                          // from within this class.


public:
    Drawable(OpenGLContext* mp_context);
    virtual ~Drawable();

    virtual void createVBOdata() = 0; // To be implemented by subclasses. Populates the VBOs of the Drawable.
    void destroyVBOdata(); // Frees the VBOs of the Drawable.

    // Getter functions for various GL data
    virtual GLenum drawMode();
    int elemCount();

    // Call these functions when you want to call glGenBuffers on the buffers stored in the Drawable
    // These will properly set the values of idxBound etc. which need to be checked in ShaderProgram::draw()

    // opaque
    void generateOIdx();
    void generateOPos();
    void generateONor();
    void generateOCol();
    void generateOUVs();
    void generateOBTs();
    void generateOBWts();
    void generateOVertData();

    bool bindOIdx();
    bool bindOPos();
    bool bindONor();
    bool bindOCol();
    bool bindOUVs();
    bool bindOBTs();
    bool bindOBWts();
    bool bindOVertData();

    // transparent
    void generateTIdx();
    void generateTPos();
    void generateTNor();
    void generateTCol();
    void generateTUVs();
    void generateTBTs();
    void generateTBWts();
    void generateTVertData();    

    bool bindTIdx();
    bool bindTPos();
    bool bindTNor();
    bool bindTCol();
    bool bindTUVs();
    bool bindTBTs();
    bool bindTBWts();
    bool bindTVertData();
};

// A subclass of Drawable that enables the base code to render duplicates of
// the Terrain class's Cube member variable via OpenGL's instanced rendering.
// You will not have need for this class when completing the base requirements
// for Mini Minecraft, but you might consider using instanced rendering for
// some of the milestone 3 ideas.
class InstancedDrawable : public Drawable {
protected:
    int m_numInstances;
    GLuint m_bufPosOffset;

    bool m_offsetGenerated;

public:
    InstancedDrawable(OpenGLContext* mp_context);
    virtual ~InstancedDrawable();
    int instanceCount() const;

    void generateOffsetBuf();
    bool bindOffsetBuf();
    void clearOffsetBuf();
    void clearColorBuf();

    virtual void createInstancedVBOdata(std::vector<glm::vec3> &offsets, std::vector<glm::vec3> &colors) = 0;
};
