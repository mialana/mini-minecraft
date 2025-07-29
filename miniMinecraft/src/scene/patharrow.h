#pragma once

#include <drawable.h>

/**
 * @brief The PathArrow class represents the 3D arrow to visualize the desired AI behavior path of 
 * mobs in the game (i.e. zombies and pigs).
 */
class PathArrow : public Drawable
{
private:
    glm::vec4 m_arrowColor;
    float m_arrowLength;

    std::vector<GLuint> m_indices;
    std::vector<glm::vec4> m_positions;
    std::vector<glm::vec4> m_normals;
    std::vector<glm::vec4> m_colors;

    /**
     * @brief helper function to build 3D arrow positions
     */
    void _generatePositions();

    /**
     * @brief helper function to build 3D arrow normals
     */
    void _generateNormals();

public:
    PathArrow(OpenGLContext* mp_context);

    /**
     * @brief changeColor
     * @param c: the color to change to
     * @return whether change was necessary and completed
     */
    bool changeColor(glm::vec4 c);

    /**
     * @brief changeLength
     * @param l: the length to change to
     * @return whether change was necessary and completed
     */
    bool changeLength(float l);

    virtual void createVBOdata() override;
};
