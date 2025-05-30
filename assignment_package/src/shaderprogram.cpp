#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <stdexcept>
#include <iostream>

ShaderProgram::ShaderProgram(OpenGLContext* context)
    : context(context)
    , vertShader()
    , fragShader()
    , prog()
    , attrPos(-1)
    , attrNor(-1)
    , attrCol(-1)
    , attrUV(-1)
    , attrBT(-1)
    , attrBWts(-1)
    , unifModel(-1)
    , unifModelInvTr(-1)
    , unifViewProj(-1)
    , unifColor(-1)
    , unifPlayerPosBWts(-1)
    , unifPlayerPos(-1)
    , unifTexture(-1)
    , unifTime(-1)
    , unifFrameBufferTexture(-1)
    , unifPlayerTexture(-1)
    , unifCamPos(-1)
{}

void ShaderProgram::create(const char* vertfile, const char* fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size() + 1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size() + 1];
    strcpy(fragSource, qFragSource.toStdString().c_str());

    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, (const char**)&vertSource, 0);
    context->glShaderSource(fragShader, 1, (const char**)&fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        printShaderInfoLog(vertShader);
    }

    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);

    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    attrUV = context->glGetAttribLocation(prog, "vs_UV");
    attrBT = context->glGetAttribLocation(prog, "vs_BT");
    attrBWts = context->glGetAttribLocation(prog, "vs_BWts");

    if (attrCol == -1) {
        attrCol = context->glGetAttribLocation(prog, "vs_ColInstanced");
    }

    attrPosOffset = context->glGetAttribLocation(prog, "vs_OffsetInstanced");

    unifModel = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor = context->glGetUniformLocation(prog, "u_Color");
    unifPlayerPosBWts = context->glGetUniformLocation(prog, "u_playerPosBiomeWts");
    unifPlayerPos = context->glGetUniformLocation(prog, "u_playerPos");
    unifTexture = context->glGetUniformLocation(prog, "u_TextureSampler");
    unifTime = context->glGetUniformLocation(prog, "u_Time");

    unifFrameBufferTexture = context->glGetUniformLocation(prog, "u_FrameBufferTexture");
    unifPlayerTexture = context->glGetUniformLocation(prog, "u_PlayerTexture");
    unifCamPos = context->glGetUniformLocation(prog, "u_CamPos");
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setModelMatrix(const glm::mat4& model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4& vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if (unifViewProj != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifViewProj,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if (unifColor != -1) {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

void ShaderProgram::setPlayerPosBiomeWts(glm::vec4 bWts)
{
    useMe();

    if (unifPlayerPosBWts != -1) {
        context->glUniform4fv(unifPlayerPosBWts, 1, &bWts[0]);
    }
}

void ShaderProgram::setPlayerPos(glm::vec3 pos)
{
    useMe();

    if (unifPlayerPos != -1) {
        context->glUniform3fv(unifPlayerPos, 1, &pos[0]);
    }
}

void ShaderProgram::setTexture(GLuint slot)
{
    useMe();

    if (slot == 0) {
        if (unifTexture != -1) {
            context->glUniform1i(unifTexture, slot);
        }
    } else if (slot == 1) {
        if (unifFrameBufferTexture != -1) {
            context->glUniform1i(unifFrameBufferTexture, slot);
        }
    } else if (slot >= 2) {
        if (unifPlayerTexture != -1) {
            context->glUniform1i(unifPlayerTexture, slot);
        }
    }
}

void ShaderProgram::setTime(int t)
{
    useMe();

    if (unifTime != -1) {
        context->glUniform1i(unifTime, t);
    }
}

void ShaderProgram::setCamPos(glm::vec3 pos)
{
    useMe();

    if (unifCamPos != -1) {
        context->glUniform3fv(unifCamPos, 1, &pos[0]);
    }
}

//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable& d)
{
    useMe();

    if (d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of "
                                + std::to_string(d.elemCount()) + "!");
    }

    if (attrPos != -1 && d.bindOPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindONor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindOCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindOIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);
    context->printGLErrorLog();

    if (attrPos != -1) {
        context->glDisableVertexAttribArray(attrPos);
    }

    if (attrNor != -1) {
        context->glDisableVertexAttribArray(attrNor);
    }

    if (attrCol != -1) {
        context->glDisableVertexAttribArray(attrCol);
    }
}

void ShaderProgram::drawInterleavedO(Drawable& d)
{
    useMe();

    if (d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of "
                                + std::to_string(d.elemCount()) + "!");
    }

    if (d.bindOVertData() && d.m_oCount > 0) {
        if (attrPos != -1) {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)0);
        }

        if (attrNor != -1) {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)sizeof(glm::vec4));
        }

        if (attrCol != -1) {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(2 * sizeof(glm::vec4)));
        }

        if (attrUV != -1) {
            context->glEnableVertexAttribArray(attrUV);
            context->glVertexAttribPointer(attrUV,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(3 * sizeof(glm::vec4)));
        }

        if (attrBT != -1) {
            context->glEnableVertexAttribArray(attrBT);
            context->glVertexAttribPointer(attrBT,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(4 * sizeof(glm::vec4)));
        }

        if (attrBWts != -1) {
            context->glEnableVertexAttribArray(attrBWts);
            context->glVertexAttribPointer(attrBWts,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(5 * sizeof(glm::vec4)));
        }

        d.bindOIdx();
        context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

        if (attrPos != -1) {
            context->glDisableVertexAttribArray(attrPos);
        }

        if (attrNor != -1) {
            context->glDisableVertexAttribArray(attrNor);
        }

        if (attrCol != -1) {
            context->glDisableVertexAttribArray(attrCol);
        }

        if (attrUV != -1) {
            context->glDisableVertexAttribArray(attrUV);
        }

        if (attrBT != -1) {
            context->glDisableVertexAttribArray(attrBT);
        }

        if (attrBWts != -1) {
            context->glDisableVertexAttribArray(attrBWts);
        }

        context->printGLErrorLog();
    }
}

void ShaderProgram::drawInterleavedT(Drawable& d)
{
    useMe();

    if (d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of "
                                + std::to_string(d.elemCount()) + "!");
    }

    if (d.bindTVertData() && d.m_tCount > 0) {
        if (attrPos != -1) {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)0);
        }

        if (attrNor != -1) {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)sizeof(glm::vec4));
        }

        if (attrCol != -1) {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(2 * sizeof(glm::vec4)));
        }

        if (attrUV != -1) {
            context->glEnableVertexAttribArray(attrUV);
            context->glVertexAttribPointer(attrUV,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(3 * sizeof(glm::vec4)));
        }

        if (attrBT != -1) {
            context->glEnableVertexAttribArray(attrBT);
            context->glVertexAttribPointer(attrBT,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(4 * sizeof(glm::vec4)));
        }

        if (attrBWts != -1) {
            context->glEnableVertexAttribArray(attrBWts);
            context->glVertexAttribPointer(attrBWts,
                                           4,
                                           GL_FLOAT,
                                           false,
                                           6 * sizeof(glm::vec4),
                                           (void*)(5 * sizeof(glm::vec4)));
        }

        d.bindTIdx();
        context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

        if (attrPos != -1) {
            context->glDisableVertexAttribArray(attrPos);
        }

        if (attrNor != -1) {
            context->glDisableVertexAttribArray(attrNor);
        }

        if (attrCol != -1) {
            context->glDisableVertexAttribArray(attrCol);
        }

        if (attrUV != -1) {
            context->glDisableVertexAttribArray(attrUV);
        }

        if (attrBT != -1) {
            context->glDisableVertexAttribArray(attrBT);
        }

        if (attrBWts != -1) {
            context->glDisableVertexAttribArray(attrBWts);
        }
    }

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName)
{
    char* text;

    if (fileName != NULL) {
        FILE* file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';  //cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char* fileName)
{
    QString text;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar* infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << "\n" << infoLog << "\n";
        delete[] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar* infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << "\n" << infoLog << "\n";
        delete[] infoLog;
    }
}
