#pragma once

#include "glm_includes.h"
# include <QMatrix4x4>
# include <QVector4D>

/// 460 linear algebra namespace. Functions used to convert glm data types to Qt data types
namespace la {
    QMatrix4x4 to_qmat(const glm::mat4 &m);
    QVector4D to_qvec(const glm::vec4 &v);
}
