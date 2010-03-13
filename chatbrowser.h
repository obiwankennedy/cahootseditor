#ifndef CHATBROWSER_H
#define CHATBROWSER_H

#include <QPlainTextEdit>

class ChatBrowser : public QPlainTextEdit
{
Q_OBJECT
public:
    ChatBrowser(QWidget *parent = 0);

signals:
    void keyPress(QKeyEvent *e);

public slots:

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // CHATBROWSER_H
