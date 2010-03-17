#include "utilities.h"

#include <QString>

// Fonts
const QString Utilities::codeFont = getSystem() == "Windows" ? "Courier New" : (getSystem() == "Mac" ? "Monaco" : "Courier 10 Pitch");
const int Utilities::codeFontSize = getSystem() == "Mac" ? 11 : 9;
const QString Utilities::labelFont = getSystem() == "Mac" ? "Lucida Grande" : "Verdana";
const int Utilities::labelFontSize = getSystem() == "Mac" ? 11 : 8;
const QString Utilities::chatFont = getSystem() == "Mac" ? "Lucida Grande" : "Verdana";
const int Utilities::chatFontSize = getSystem() == "Mac" ? 11 : 8;

QString Utilities::getSystem() {
    #ifdef Q_WS_X11
    return QString("Linux");
    #endif

    #ifdef Q_WS_MAC
    return QString("Mac");
    #endif

    #ifdef Q_WS_QWS
    return QString("Embedded Linux");
    #endif

    #ifdef Q_WS_WIN
    return QString("Windows");
    #endif
}

