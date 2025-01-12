#include "mainwindow.h"
#include "myutils.h"

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QSurfaceFormat>

QDebug operator<<(QDebug qDebugInstance, const auto& data) {
    QDebugStateSaver saver(qDebugInstance);
    qDebugInstance.nospace() << data;
    return qDebugInstance;
}

QString getTimestamp() {
    QString s = "[";

    QDateTime dateTime = QDateTime::currentDateTime();
    QString dtString = dateTime.toString(Qt::DateFormat ::ISODateWithMs);
    dtString = dtString.replace('T', ' ');
    QString timeZone = dateTime.timeZoneAbbreviation();

    s += dtString + " " + timeZone + "] ";
    return s;
}

void handleQLogging(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QString timeStr = getTimestamp();

    std::string file = context.file ? std::string(context.file) : "";
    std::string function = context.function ? std::string(context.function) : "";
    std::string line = context.line ? std::to_string(context.line) : "";
    std::string location = std::string(console::black) + " [" + file + ":" + line + " in `"
                           + function + "`]";

    // location << console::black << msg;

    std::string end = "\n";

    std::ostringstream fullMsg;
    fullMsg << qPrintable(timeStr) << qPrintable(msg);

    switch (type) {
        case QtInfoMsg: std::clog << console::green << "[INFO] " << fullMsg.str() << end; break;
        case QtDebugMsg:
            std::clog << console::cyan << "[DEBUG] " << fullMsg.str() << location << end;
            break;
        case QtWarningMsg:
            std::cerr << console::yellow << "[WARN] " << fullMsg.str() << location << end;
            break;
        case QtCriticalMsg:
            std::cerr << console::orange << "[CRITICAL] " << fullMsg.str() << location << end;
            break;
        case QtFatalMsg:
            std::cerr << console::red << "[FATAL] " << fullMsg.str() << location << end;
            break;
    }
}

std::string getVersionString(std::pair<int, int> version) {
    return std::to_string(version.first) + "." + std::to_string(version.second);
}

const char* getProfileString(QSurfaceFormat::OpenGLContextProfile prof) {
    return prof == QSurfaceFormat::CoreProfile            ? "Core"
           : prof == QSurfaceFormat::CompatibilityProfile ? "Compatibility"
                                                          : "None";
}

void setOpenGLFormat() {
    QSurfaceFormat format;

    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);  // deprecated gl not supported
    std::pair<int, int> version = std::make_pair(4, 0);
    QSurfaceFormat::OpenGLContextProfile profile = QSurfaceFormat::CoreProfile;
    int samples = 4;

    format.setVersion(version.first, version.second);  // Set OpenGL 4.0
    format.setProfile(profile);
    format.setSamples(samples);  // for 4-sample antialiasing. not always supported.

    /*** Check whether automatic testing is enabled */
    if (qgetenv("CIS277_AUTOTESTING") != nullptr) {
        format.setSamples(0);
    }

    QSurfaceFormat::setDefaultFormat(format);

    QSurfaceFormat newFormat = QSurfaceFormat::defaultFormat();
    std::pair<int, int> newVersion = format.version();
    QSurfaceFormat::OpenGLContextProfile newProfile = format.profile();
    int newSamples = format.samples();

    qInfo() << "OpenGL Format Info: (expected | actual)";
    console::qInfoNq() << "  Version:" << getVersionString(version) << "|"
                       << getVersionString(newVersion);
    qInfo() << "  Profile:" << getProfileString(profile) << "|" << getProfileString(newProfile);
    qInfo() << "  Samples:" << samples << "|" << newSamples;
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(handleQLogging);

    QApplication a(argc, argv);

#ifdef __cplusplus
    qInfo() << "C++ version:" << __cplusplus;
#endif

    setOpenGLFormat();

    MainWindow w;
    w.show();

    return a.exec();
}
