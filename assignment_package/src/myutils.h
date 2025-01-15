#pragma once

#include <QDebug>

enum Code {
    FG_GREEN = 32,
    FG_BLUE = 34,
};

namespace console
{
extern const char* black;
extern const char* red;
extern const char* green;
extern const char* yellow;
extern const char* cyan;

extern const char* orange;  // not available on some systems

extern const char* reset;

void setColor(Code code);
void resetAll();

QDebug qInfoNq();  // returns `qInfo().noquote()`

QDebug qDebugNq();  // returns `qDebug().noquote()`

}  // namespace console

// namespace Color {
// enum Code {
//     FG_RED      = 31,
//     FG_GREEN    = 32,
//     FG_BLUE     = 34,
//     FG_DEFAULT  = 39,
//     BG_RED      = 41,
//     BG_GREEN    = 42,
//     BG_BLUE     = 44,
//     BG_DEFAULT  = 49
// };
// class Modifier {
//     Code code;
// public:
//     Modifier(Code pCode) : code(pCode) {}
//     friend std::ostream&
//     operator<<(std::ostream& os, const Modifier& mod) {
//         return os << "\033[" << mod.code << "m";
//     }
// };
// }
