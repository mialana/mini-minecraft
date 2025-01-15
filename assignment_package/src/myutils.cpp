#include "myutils.h"
#include <iostream>

const char* console::black("\033[0;30m");
const char* console::red("\033[0;31m");
const char* console::green("\033[0;32m");
const char* console::yellow("\033[0;33m");
const char* console::cyan("\033[0;36m");

const char* console::orange("\033[38;5;208m");  // not available on some systems

const char* console::reset("\033[0m");

QDebug console::qInfoNq() {
    return qInfo().noquote();
}

QDebug console::qDebugNq() {
    return qDebug().noquote();
}

void console::resetAll() {
    std::cout << "\033[0m" << std::flush;
}

void console::setColor(Code code) {
    std::cout << "\033[0;" << code << "m" << std::flush;
}
