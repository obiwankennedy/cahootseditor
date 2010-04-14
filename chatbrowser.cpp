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
