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
#ifndef CHATBROWSER_H
#define CHATBROWSER_H

#include <QTextEdit>

class ChatBrowser : public QTextEdit
{
Q_OBJECT
public:
    ChatBrowser(QWidget *parent = 0);

    void addChatLine(QString str, QColor color = Qt::black);

signals:
    void keyPress(QKeyEvent *e);

public slots:

private:
    void keyPressEvent(QKeyEvent *e);
};

#endif // CHATBROWSER_H
