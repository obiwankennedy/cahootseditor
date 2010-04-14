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
