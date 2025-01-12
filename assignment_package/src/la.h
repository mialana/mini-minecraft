#pragma once

#include "glm_includes.h"
#include <QMatrix4x4>
#include <QVector4D>

/// 460 linear algebra namespace. Functions used to convert glm data types to Qt
/// data types
namespace la
{
QMatrix4x4 to_qmat(const glm::mat4& m);
QVector4D to_qvec(const glm::vec4& v);
}  // namespace la

namespace console
{
const std::string black("\033[0;30m");
const std::string red("\033[0;31m");
const std::string green("\033[0;32m");
const std::string yellow("\033[0;33m");
const std::string cyan("\033[0;36m");

const std::string orange("\033[38;5;208m");  // not available on some systems

const std::string reset("\033[0m");
}  // namespace console
