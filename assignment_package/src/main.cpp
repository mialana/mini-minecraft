#include "mainwindow.h"

#include "la.h"
#include <iomanip>
#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QSurfaceFormat>

QDebug operator<<(QDebug qDebugInstance, const auto& data) {
    QDebugStateSaver saver(qDebugInstance);
    qDebugInstance.nospace() << data;
    return qDebugInstance;
}

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    std::string file = context.file ? std::string(context.file) : "";
    std::string function = context.function ? std::string(context.function) : "";
    std::string line = context.line ? std::to_string(context.line) : "";
    std::string loc = console::black + " [" + file + ":" + line + " in `" + function + "`]";

    QString timeStr = qFormatLogMessage(type, context, msg);

    std::string end = console::reset + "\n";

    std::string fullMsg = qPrintable(timeStr);

    switch (type) {
        case QtInfoMsg:
            std::clog << console::green << "[INFO] " << fullMsg << qPrintable(msg) << end;
            break;
        case QtDebugMsg:
            std::clog << console::cyan << "[DEBUG] " << fullMsg << std::setw(16) << loc << end;
            break;

        case QtWarningMsg:
            std::cerr << console::yellow << "[WARN] " << fullMsg << std::setw(16) << loc << end;
            break;
        case QtCriticalMsg:
            std::cerr << console::orange << "[CRITICAL] " << fullMsg << std::setw(16) << loc << end;
            break;
        case QtFatalMsg:
            std::cerr << console::red << "[FATAL] " << fullMsg << std::setw(16) << loc << end;
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

    qInfo().setQuoteStrings(false);
    std::cout << qInfo().quoteStrings() << std::endl;

    qInfo() << "OpenGL Format Info:";
    qInfo().noquote() << "  Version:" << getVersionString(newVersion);
    qInfo() << "  Profile:" << getProfileString(newProfile);
    qInfo() << "  Samples:" << newSamples;
    qDebug() << "Debugging" << samples;
    qWarning() << "Debugging" << samples;
    qCritical() << "Critical" << samples;
    qFatal() << "Fatal" << samples;
}

int main(int argc, char* argv[]) {
    qSetMessagePattern("[%{time yyyy-MM-dd h:mm:ss.zzz t}] ");
    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);

#ifdef __cplusplus
    qInfo() << "C++ version: " << __cplusplus;
#endif

    setOpenGLFormat();

    MainWindow w;
    w.show();

    return a.exec();
}
