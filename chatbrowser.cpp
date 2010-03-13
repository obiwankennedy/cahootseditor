#include "chatbrowser.h"

ChatBrowser::ChatBrowser(QWidget *parent) : QPlainTextEdit(parent)
{
    setReadOnly(true);
}

void ChatBrowser::keyPressEvent(QKeyEvent *e)
{
    emit keyPress(e);
}
