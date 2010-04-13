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
#include <QSettings>

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
    void announceDocument(bool broadcastDocument);

    void connectToDocument(QStringList list);

    void changeEditorFont(QString fontString);
    void changeChatFont(QString fontString);
    void changeParticipantsFont(QString fontString);

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    // shows/hides the participants pane
    void setParticipantsHidden(bool b);
    // shows/hides the chat pane
    void setChatHidden(bool b);
    // shifts the current line/selected lines left
    void shiftLeft();
    // shifts the current line/selected lines right
    void shiftRight();
    // C++ style (un)commenting
    void unCommentSelection();

    // User wants to resynchronize the document with the owner
    void resynchronizeTriggered();

    // Sets the highlighting style to the below Highlighter
    void setHighlighter(int Highlighter);
    enum Highlighter {None, CPlusPlus, Python};

    // returns if the editor is undable
    bool isUndoable();
    // returns if the editor is redoable
    bool isRedoable();
    // returns if the editor is modified (unsaved since last edit)
    bool isModified();
    // returns if the chat pane is hidden, used for determining which actions to enable/disable by the main window
    bool isChatHidden();
    // returns if the participants pane is hidden, used for determining which actions to enable/disable in the MW
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

    // Toggles line wrapping in the editor
    void toggleLineWrap();

    // Used for setting the document's modified attribute to b (for the modified-since-last-save attribute)
    void setModified(bool b);

    // Previews the current document as an HTML webpage in a dialog box
    void previewAsHtml();

    // Splits the editor vertically from horizontal or no split
    void splitEditor();
    // Splits the editor horizontally from vertical or no split
    void splitEditorSideBySide();
    // Unsplits the editor
    void unSplitEditor();
    bool isEditorSplit();
    bool isEditorSplitSideBySide();

#warning "rework this for the main window"
    // returns if the document has been announced
    bool isAnnounced();
    // returns if the document has connected to another document previously
    bool isConnected();
    void setOwnerName(QString name);

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
