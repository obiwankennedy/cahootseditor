#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtGui/QWidget>
#include "highlighters/cpphighlighter.h"

#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
    class Document;
}

class Document : public QWidget {
    Q_OBJECT
public:
    Document(QWidget *parent = 0);
    ~Document();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void setParticipantsHidden(bool b);
    void setChatHidden(bool b);

    bool isUndoable();
    bool isRedoable();
    bool isModified();
    bool isChatHidden();
    bool isParticipantsHidden();

    QString getPlainText();
    void setPlainText(QString text);

    void setModified(bool b);

    QString curFile;
    
private:
    Ui::Document *ui;

    QTcpServer *server;
    QTcpSocket *socket;

    CppHighlighter *cppHighlighter;

    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onNewConnection();
    void on_pushButton_clicked();

signals:
    void redoAvailable(bool);
    void undoAvailable(bool);
};

#endif // DOCUMENT_H
