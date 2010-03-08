#ifndef CHATPANE_H
#define CHATPANE_H

#include <QWidget>

namespace Ui {
    class ChatPane;
}

class ChatPane : public QWidget {
    Q_OBJECT
public:
    ChatPane(QWidget *parent = 0);
    ~ChatPane();

    void appendChatMessage(QString str);

    bool hasFocus() const;

    void undo();
    void redo();

    void cut();
    void copy();
    void paste();

private:
    Ui::ChatPane *ui;

private slots:
    void on_lineEdit_returnPressed();

signals:
    void returnPressed(QString str);
};

#endif // CHATPANE_H
