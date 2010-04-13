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

    void changeFont(QString fontString);

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
