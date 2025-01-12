#pragma once

#include <QDebug>

namespace console
{
extern const char* black;
extern const char* red;
extern const char* green;
extern const char* yellow;
extern const char* cyan;

extern const char* orange;  // not available on some systems

extern const char* reset;

QDebug qInfoNq();  // returns `qInfo().noquote()`

QDebug qDebugNq();  // returns `qDebug().noquote()`

}  // namespace console
