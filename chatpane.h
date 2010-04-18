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
#ifndef CHATPANE_H
#define CHATPANE_H

#include <QWidget>

#include "chatbrowser.h"

namespace Ui {
    class ChatPane;
}

class ChatPane : public QWidget {
    Q_OBJECT
public:
    ChatPane(QWidget *parent = 0);
    ~ChatPane();

    void appendChatMessage(QString str, QColor color = Qt::black);

    bool hasFocus() const;

    void undo();
    void redo();

    void cut();
    void copy();
    void paste();

    void setReadOnly(bool ro);

    void setFont(QFont font);

private:
    Ui::ChatPane *ui;

    ChatBrowser *chatBrowser;

private slots:
    void on_lineEdit_returnPressed();

    void textBrowserKeyPress(QKeyEvent *event);

signals:
    void returnPressed(QString str);
};

#endif // CHATPANE_H
