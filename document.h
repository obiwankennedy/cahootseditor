#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtGui/QWidget>
#include "highlighters/cpphighlighter.h"
#include "codeeditor.h"
#include "participantspane.h"
#include "chatpane.h"

#include "client.h"

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
    void setParticipantsHidden(bool b);
    void setChatHidden(bool b);
    void shiftLeft();
    void shiftRight();
    void unCommentSelection();

    // This sets up the document so people can connect to it.
    // Hopefully we can do something with Bonjour so you can browse for local documents
    // but that's down the road.
    void announceDocument();

    bool isUndoable();
    bool isRedoable();
    bool isModified();
    bool isChatHidden();
    bool isParticipantsHidden();

    // Find functions
    void findNext(QString str, bool ignoreCase, bool wrapAround);
    void findPrev(QString str, bool ignoreCase, bool wrapAround);

    QString getPlainText();
    void setPlainText(QString text);
    void toggleLineWrap();

    void setModified(bool b);

    void previewAsHtml();

    void splitEditor();
    bool isEditorSplit();
    bool isAnnounced();

    // This tells us if we're the host/owner of the document,
    // and affects how we talk with participants
    bool isOwner;
    QString curFile;
    
    QString myName;

private:
    Ui::Document *ui;

    QTcpServer *server;
    QTcpSocket *socket;
    Client *client;

    QList<QTcpSocket *> clientList;

    CppHighlighter *cppHighlighter;

    CodeEditor *editor;
    CodeEditor *bottomEditor;
    bool isAlreadySplit;
    bool isAlreadyAnnounced;

    ParticipantsPane *participantPane;

    ChatPane *chatPane;

    void ownerIncomingData(QString data);
    void participantIncomingData(QString data);

private slots:
    // The elephant in the room. Handles collaborative editing.
    void onTextChange(int pos, int charsRemoved, int charsAdded);

    void onChatSend(QString str);
    void onIncomingData();
    void onNewConnection();
    void socketStateChanged(QAbstractSocket::SocketState state);

    void disconnected();

signals:
    void redoAvailable(bool);
    void undoAvailable(bool);
};

#endif // DOCUMENT_H
