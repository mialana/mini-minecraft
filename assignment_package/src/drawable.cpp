#include "drawable.h"
#include <mygl.h>

Drawable::Drawable(MyGL& context)
    : m_oCount(-1),
      m_oBufIdx(),
      m_oBufPos(),
      m_oBufNor(),
      m_oBufCol(),
      m_oBufUVs(),
      m_oBufBTs(),
      m_oBufBWts(),
      m_oBufVertData(),
      m_oIdxGenerated(false),
      m_oPosGenerated(false),
      m_oNorGenerated(false),
      m_oColGenerated(false),
      m_oUVsGenerated(false),
      m_oBTsGenerated(false),
      m_oBWtsGenerated(false),
      m_oVertDataGenerated(false),
      m_tCount(-1),
      m_tBufIdx(),
      m_tBufPos(),
      m_tBufNor(),
      m_tBufCol(),
      m_tBufUVs(),
      m_tBufBTs(),
      m_tBufBWts(),
      m_tBufVertData(),
      m_tIdxGenerated(false),
      m_tPosGenerated(false),
      m_tNorGenerated(false),
      m_tColGenerated(false),
      m_tUVsGenerated(false),
      m_tBTsGenerated(false),
      m_tBWtsGenerated(false),
      m_tVertDataGenerated(false),
      mr_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroyVBOdata()
{
    mr_context.glDeleteBuffers(1, &m_oBufIdx);
    mr_context.glDeleteBuffers(1, &m_oBufPos);
    mr_context.glDeleteBuffers(1, &m_oBufNor);
    mr_context.glDeleteBuffers(1, &m_oBufCol);
    mr_context.glDeleteBuffers(1, &m_oBufBWts);
    mr_context.glDeleteBuffers(1, &m_oBufVertData);
    m_oIdxGenerated = m_oPosGenerated = m_oNorGenerated = m_oColGenerated = m_oBWtsGenerated = m_oVertDataGenerated = false;
    m_oCount = -1;

    mr_context.glDeleteBuffers(1, &m_tBufIdx);
    mr_context.glDeleteBuffers(1, &m_tBufPos);
    mr_context.glDeleteBuffers(1, &m_tBufNor);
    mr_context.glDeleteBuffers(1, &m_tBufCol);
    mr_context.glDeleteBuffers(1, &m_tBufBWts);
    mr_context.glDeleteBuffers(1, &m_tBufVertData);
    m_tIdxGenerated = m_tPosGenerated = m_tNorGenerated = m_tColGenerated = m_oBWtsGenerated = m_tVertDataGenerated = false;
    m_tCount = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_oCount;
}

void Drawable::generateOIdx()
{
    m_oIdxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mr_context.glGenBuffers(1, &m_oBufIdx);
}

void Drawable::generateOPos()
{
    m_oPosGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mr_context.glGenBuffers(1, &m_oBufPos);
}

void Drawable::generateONor()
{
    m_oNorGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mr_context.glGenBuffers(1, &m_oBufNor);
}

void Drawable::generateOCol()
{
    m_oColGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mr_context.glGenBuffers(1, &m_oBufCol);
}

void Drawable::generateOUVs() {
    m_oUVsGenerated = true;
    mr_context.glGenBuffers(1, &m_oBufUVs);
}

void Drawable::generateOBTs() {
    m_oBTsGenerated = true;
    mr_context.glGenBuffers(1, &m_oBufBTs);
}

void Drawable::generateOBWts() {
    m_oBWtsGenerated = true;
    mr_context.glGenBuffers(1, &m_oBufBWts);
}

void Drawable::generateOVertData()
{
    m_oVertDataGenerated = true;
    // Create a VBO on our GPU and store its handle in bufVertData
    mr_context.glGenBuffers(1, &m_oBufVertData);
}

bool Drawable::bindOIdx()
{
    if(m_oIdxGenerated) {
        mr_context.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_oBufIdx);
    }
    return m_oIdxGenerated;
}

bool Drawable::bindOPos()
{
    if(m_oPosGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufPos);
    }
    return m_oPosGenerated;
}

bool Drawable::bindONor()
{
    if(m_oNorGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufNor);
    }
    return m_oNorGenerated;
}

bool Drawable::bindOCol()
{
    if(m_oColGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufCol);
    }
    return m_oColGenerated;
}

bool Drawable::bindOUVs()
{
    if(m_oUVsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufUVs);
    }
    return m_oUVsGenerated;
}

bool Drawable::bindOBTs()
{
    if(m_oBTsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufBTs);
    }
    return m_oBTsGenerated;
}

bool Drawable::bindOBWts() {
    if(m_oBWtsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufBWts);
    }
    return m_oBWtsGenerated;
}

bool Drawable::bindOVertData()
{
    if(m_oVertDataGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_oBufVertData);
    }
    return m_oVertDataGenerated;
}

void Drawable::generateTIdx()
{
    m_tIdxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mr_context.glGenBuffers(1, &m_tBufIdx);
}

void Drawable::generateTPos()
{
    m_tPosGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mr_context.glGenBuffers(1, &m_tBufPos);
}

void Drawable::generateTNor()
{
    m_tNorGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mr_context.glGenBuffers(1, &m_tBufNor);
}

void Drawable::generateTCol()
{
    m_tColGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mr_context.glGenBuffers(1, &m_tBufCol);
}

void Drawable::generateTUVs() {
    m_tUVsGenerated = true;
    mr_context.glGenBuffers(1, &m_tBufUVs);
}

void Drawable::generateTBTs() {
    m_tBTsGenerated = true;
    mr_context.glGenBuffers(1, &m_tBufBTs);
}

void Drawable::generateTBWts() {
    m_tBWtsGenerated = true;
    mr_context.glGenBuffers(1, &m_tBufBWts);
}

void Drawable::generateTVertData()
{
    m_tVertDataGenerated = true;
    // Create a VBO on our GPU and store its handle in bufVertData
    mr_context.glGenBuffers(1, &m_tBufVertData);
}

bool Drawable::bindTIdx()
{
    if(m_tIdxGenerated) {
        mr_context.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_tBufIdx);
    }
    return m_tIdxGenerated;
}

bool Drawable::bindTPos()
{
    if(m_tPosGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufPos);
    }
    return m_tPosGenerated;
}

bool Drawable::bindTNor()
{
    if(m_tNorGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufNor);
    }
    return m_tNorGenerated;
}

bool Drawable::bindTCol()
{
    if(m_tColGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufCol);
    }
    return m_tColGenerated;
}

bool Drawable::bindTUVs()
{
    if(m_tUVsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufUVs);
    }
    return m_tUVsGenerated;
}

bool Drawable::bindTBTs()
{
    if(m_tBTsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufBTs);
    }
    return m_tBTsGenerated;
}

bool Drawable::bindTBWts() {
    if(m_tBWtsGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufBWts);
    }
    return m_tBWtsGenerated;
}

bool Drawable::bindTVertData()
{
    if(m_tVertDataGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_tBufVertData);
    }
    return m_tVertDataGenerated;
}

InstancedDrawable::InstancedDrawable(MyGL& context)
    : Drawable(context), m_numInstances(0), m_bufPosOffset(-1), m_offsetGenerated(false)
{}

InstancedDrawable::~InstancedDrawable(){}

int InstancedDrawable::instanceCount() const {
    return m_numInstances;
}

void InstancedDrawable::generateOffsetBuf() {
    m_offsetGenerated = true;
    mr_context.glGenBuffers(1, &m_bufPosOffset);
}

bool InstancedDrawable::bindOffsetBuf() {
    if(m_offsetGenerated){
        mr_context.glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    }
    return m_offsetGenerated;
}


void InstancedDrawable::clearOffsetBuf() {
    if(m_offsetGenerated) {
        mr_context.glDeleteBuffers(1, &m_bufPosOffset);
        m_offsetGenerated = false;
    }
}
void InstancedDrawable::clearColorBuf() {
    if(m_oColGenerated) {
        mr_context.glDeleteBuffers(1, &m_oBufCol);
        m_oColGenerated = false;
    }
}
