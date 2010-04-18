/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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

