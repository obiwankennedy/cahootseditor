#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>

class Utilities
{
public:
    // Fonts:
    static const QString codeFont;
    static const int codeFontSize;
    static const QString labelFont;
    static const int labelFontSize;
    static const QString chatFont;
    static const int chatFontSize;

    static QString getSystem();

};

#endif // UTILITIES_H
