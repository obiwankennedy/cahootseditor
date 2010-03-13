#include "chatbrowser.h"

ChatBrowser::ChatBrowser(QWidget *parent) : QPlainTextEdit(parent)
{
    setReadOnly(true);
}

void ChatBrowser::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() == Qt::NoModifier) {
        emit keyPress(e);
    }
}
