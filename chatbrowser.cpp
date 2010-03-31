#include "chatbrowser.h"

#include <QTextBlock>
#include <QTextCharFormat>

ChatBrowser::ChatBrowser(QWidget *parent) : QPlainTextEdit(parent)
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
    appendPlainText(str);
    return;
#warning "implement, would be nice to have colors mean something in the chat pane."
    QTextCursor cursor = QTextCursor(this->document());
    cursor.setPosition(this->document()->lastBlock().position());
    cursor.select(QTextCursor::LineUnderCursor);
    QTextCharFormat format;
    (void)format;
    (void)color;
}
