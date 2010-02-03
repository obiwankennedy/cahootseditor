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

    void connectToDocument(QStringList *list);

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void find();
    void setParticipantsHidden(bool b);
    void setChatHidden(bool b);
    void shiftLeft();
    void shiftRight();
    void comment();

    // This sets up the document so people can connect to it.
    // Hopefully we can do something with Bonjour so you can browse for local documents
    // but that's down the road.
    void announceDocument();

    bool isUndoable();
    bool isRedoable();
    bool isModified();
    bool isChatHidden();
    bool isParticipantsHidden();

    QString getPlainText();
    void setPlainText(QString text);
    void toggleLineWrap();

    void setModified(bool b);

    // This tells us if we're the host/owner of the document, and affects how we talk with participants
    bool isOwner;
    QString curFile;
    
private:
    Ui::Document *ui;

    QTcpServer *server;
    QTcpSocket *socket;

    QList<QTcpSocket *> clientList;
    QMap<QObject*, QString> socketToNameMap;

    QString myName;

    CppHighlighter *cppHighlighter;

    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void on_pushButton_clicked();
    void onIncomingData();
    void onNewConnection();

signals:
    void redoAvailable(bool);
    void undoAvailable(bool);
};

#endif // DOCUMENT_H
