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
#include "chatbrowser.h"

#include <QTextBlock>
#include <QKeyEvent>
#include <QTextCharFormat>

ChatBrowser::ChatBrowser(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
}

void ChatBrowser::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ShiftModifier) {
        emit keyPress(e);
    }
}

void ChatBrowser::addChatLine(QString str, QColor color)
{
    QRegExp rx("([a-zA-Z0-9_]*):\t(.*)");
    if (str.contains(rx)) {
        str = "<b>" + rx.cap(1) + ":</b>\t" + rx.cap(2);
    }
    else {
        str = "<b>" + str + "</b>";
    }
    append(str);
    return;
#warning "implement, would be nice to have colors mean something in the chat pane."
    QTextCursor cursor = QTextCursor(this->document());
    cursor.setPosition(this->document()->lastBlock().position());
    cursor.select(QTextCursor::LineUnderCursor);
    QTextCharFormat format;
    (void)format;
    (void)color;
}
