#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtGui/QWidget>
#include "highlighters/cpphighlighter.h"
#include "highlighters/pythonhighlighter.h"
#include "codeeditor.h"
#include "participantspane.h"
#include "chatpane.h"
#include "enu.h"
#include "findtoolbar.h"

#include "client.h"
#include "server.h"

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

    // This sets up the document so people can connect to it.
    // Hopefully we can do something with Bonjour so you can browse for local documents
    // but that's down the road.
    void announceDocument();

    void connectToDocument(QStringList list);

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

    void resynchronizeTriggered();

    void setHighlighter(int Highlighter);
    enum Highlighter {None, CPlusPlus, Python};

    bool isUndoable();
    bool isRedoable();
    bool isModified();
    bool isChatHidden();
    bool isParticipantsHidden();

    // Find functions
    void findAll();
    void findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    void findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    void replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity,Enu::FindMode mode);
    void replace(QString replaceString);
    void findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);

    QString getPlainText();
    void setPlainText(QString text);
    void toggleLineWrap();

    void setModified(bool b);

    void previewAsHtml();

    void splitEditor();
    void splitEditorSideBySide();
    void unSplitEditor();
    bool isEditorSplit();
    bool isEditorSplitSideBySide();
    bool isAnnounced();
    bool isConnected();

    // This tells us if we're the host/owner of the document,
    // and affects how we talk with participants
    Enu::Permissions myPermissions;
    QString curFile;
    
    QString myName;

private:
    Ui::Document *ui;

    Client *client;
    Server *server;

    QSyntaxHighlighter *highlighter;

    CodeEditor *editor;
    CodeEditor *bottomEditor;
    bool isAlreadyAnnounced;
    bool isAlreadyConnected;

    FindToolBar *findAllToolbar;
    ParticipantsPane *participantPane;
    ChatPane *chatPane;

private slots:
    // triggered by the find toolbar, not the dialog
    void findNext(QString string);
    void findPrevious(QString string);

signals:
    void redoAvailable(bool);
    void undoAvailable(bool);
    void notFound();
};

#endif // DOCUMENT_H
