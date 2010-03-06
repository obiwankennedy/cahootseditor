#include "document.h"
#include "ui_document.h"

#include <QDialog>
#include <QKeyEvent>
#include <QTextCursor>
#include <QRegExp>
#include <QDebug>
#include <QWheelEvent>
#include <QTextEdit>
#include <QTextBlock>
#include <QFontMetrics>
#include <QWebView>
#include <QLayout>

#include <QTextDocumentFragment>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    ui->bottomEditorFrame->hide();

    isAlreadySplit = false;

    delete ui->editorFrame;
    editor = new CodeEditor(this);
    editor->setFont(QFont("Monaco", 11));
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, editor);

    cppHighlighter = new CppHighlighter(editor->document());

    delete ui->participantFrame;
    participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, participantPane);

    delete ui->chatFrame;
    chatPane = new ChatPane();
    ui->codeChatSplitter->insertWidget(1, chatPane);

    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(chatPane, SIGNAL(returnPressed(QString)), this, SLOT(onChatSend(QString)));


    server = new QTcpServer(this);
    socket = new QTcpSocket(this);
//    client = new Client();

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);


    myName = "Owner"; // temporary

    isOwner = true; // We are the document owner, unless we're connecting to someone elses document
}

Document::~Document()
{
    delete ui;
}

void Document::connectToDocument(QStringList *list)
{
#warning "check this better."
    isOwner = false;
    if (list->size() == 3) { // More thorough checking is needed to ensure the contents are usable.
        myName = list->at(0);
        QString address = list->at(1);
        QString portString = list->at(2);
        int port = portString.toInt();
        socket->connectToHost(QHostAddress(address), port);
//        client->socket->connectToHost(QHostAddress(address), port);
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
//        connect(client->socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        setChatHidden(false);
        setParticipantsHidden(false);
        participantPane->setConnectInfo(QString("%1:%2").arg(address).arg(portString));
        delete list;
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    }
}

void Document::undo()
{
    editor->undo();
}

void Document::redo()
{
    editor->redo();
}

void Document::cut()
{
    editor->cut();
}

void Document::copy()
{
    editor->copy();
}

void Document::paste()
{
    editor->paste();
}

void Document::setParticipantsHidden(bool b)
{
    if (b) {
        ui->participantSplitter->widget(1)->hide();
    }
    else {
        ui->participantSplitter->widget(1)->show();
    }
}

void Document::setChatHidden(bool b)
{
    // Hide/show the widget (contains the chat widget) below the code text edit
    if (b) {
        ui->codeChatSplitter->widget(1)->hide();
    }
    else {
        ui->codeChatSplitter->widget(1)->show();
    }
}

void Document::shiftLeft()
{
    editor->shiftLeft();
}

void Document::shiftRight()
{
    editor->shiftRight();
}

void Document::unCommentSelection()
{
    editor->unCommentSelection();
}

void Document::announceDocument()
{
    setChatHidden(false);
    setParticipantsHidden(false);

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    server->listen(QHostAddress::Any, 0); // Port is chosen automatically, listening on all NICs
    QString port = QString::number(server->serverPort(), 10);

    chatPane->appendChatMessage("Listening on port " + port);

    participantPane->setConnectInfo(port);
}

bool Document::isUndoable()
{
    return editor->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return editor->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return editor->document()->isModified();
}

bool Document::isChatHidden()
{
    return ui->codeChatSplitter->widget(1)->isHidden();
}

bool Document::isParticipantsHidden()
{
    return ui->participantSplitter->widget(1)->isHidden();
}

void Document::findNext(QString str, bool ignoreCase, bool wrapAround)
{
    (void)ignoreCase;
    (void)wrapAround;
    editor->document()->find(str, editor->textCursor());
}

void Document::findPrev(QString str, bool ignoreCase, bool wrapAround)
{
    (void)ignoreCase;
    (void)wrapAround;
    editor->document()->find(str, editor->textCursor());
    editor->setFocus();
}

QString Document::getPlainText()
{
    return editor->toPlainText();
}

void Document::setPlainText(QString text)
{
    editor->setPlainText(text);
}

void Document::toggleLineWrap()
{
    if (editor->lineWrapMode() == QPlainTextEdit::NoWrap) {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void Document::setModified(bool b)
{
    editor->document()->setModified(b);
}

void Document::previewAsHtml()
{
    QString text = editor->toPlainText();
    QWebView *preview = new QWebView();
    preview->setHtml(text);
    preview->show();
}

void Document::splitEditor()
{
    if (isEditorSplit()) {
        return;
    }
    else {
        isAlreadySplit = true;
        delete ui->bottomEditorFrame;
        bottomEditor = new CodeEditor(this);
        bottomEditor->setFont(editor->font());
        QFontMetrics fm(editor->font());
        bottomEditor->setTabStopWidth(fm.averageCharWidth() * 4);
        ui->editorSplitter->insertWidget(1, bottomEditor);
        bottomEditor->document()->deleteLater();
        bottomEditor->setDocument(editor->document());
    }
}

bool Document::isEditorSplit()
{
    return isAlreadySplit;
}

void Document::ownerIncomingData(QString data)
{
    qDebug() << "odata: " << data;
    QString toSend;
    if (data.startsWith("doc:")) {
        toSend = data;
        data.remove(0, 4);
        // detect line number, then put text at that line.
        QRegExp rx = QRegExp("^(\\d+),(\\d+),(\\d+):(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            QString data = rx.cap(4);
            editor->collabTextChange(pos, charsRemoved, charsAdded, data);
        }
    }
    else {
        toSend = QString("Someone: %2").arg(data);
        chatPane->appendChatMessage(toSend);
    }
    // Distribute data to all the other participants

    for (int i = 0; i < clientList.size(); i++) {
        if (clientList.at(i) != sender()) {
            clientList.at(i)->write(toSend.toAscii());
        }
    }
}

void Document::participantIncomingData(QString data)
{
    qDebug() << "pdata: " << data;
    if (data.startsWith("doc:")) {
        data.remove(0, 4);
        // detect line number, then put text at that position.
        QRegExp rx = QRegExp("^(\\d+),(\\d+),(\\d+):(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            QString data = rx.cap(4);
            editor->collabTextChange(pos, charsRemoved, charsAdded, data);
        }
    }
    else {
        chatPane->appendChatMessage(data);
    }

}

void Document::onTextChange(int pos, int charsRemoved, int charsAdded)
{
    if (!isOwner && socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QString data;
    if (charsRemoved > 0 && charsAdded == 0) {
        data = "";
    }
    else if (charsAdded > 0) {
        QTextCursor cursor = QTextCursor(editor->document());
        cursor.setPosition(pos, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        data = cursor.selection().toPlainText();
    }

    if (isOwner) {
        QString toSend = QString("doc:%1,%2,%3:%4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);
        for (int i = 0; i < clientList.size(); i++) {
            clientList.at(i)->write(toSend.toAscii());
        }
    }
    else {
        socket->write(QString("doc:%1,%2,%3:%4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data).toAscii());
    }
}

void Document::onChatSend(QString str)
{
    if (isOwner) {
        for (int i = 0; i < clientList.size(); i++) {
            clientList.at(i)->write(QString(myName + ": ").toAscii() + str.toAscii());
        }
    }
    else {
        socket->write(str.toAscii());
    }
    chatPane->appendChatMessage(myName + ": " + str);
}

void Document::onIncomingData()
{
    // disconnect the signal that fires when the contents of the editor change so we don't echo
    disconnect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    QString data;
    if (isOwner) {
        // We know we have incoming data, so iterate through our current participants to find the correct sender
        for (int i = 0; i < clientList.size(); i++) {
            if (sender() == clientList.at(i)) {
                // sender() is the sender of the signal that calls this slot
                data = clientList.at(i)->readAll();
                ownerIncomingData(data);
            }
        }
    }
    else { // We are a participant
        data = socket->readAll();
        participantIncomingData(data);
    }
    // reconnect the signal that fires when the contents of the editor change so we continue to send new text
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
}

void Document::onNewConnection()
{
    if (isOwner) {
        clientList.append(server->nextPendingConnection());
        connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        participantPane->insertParticipant("Newbie");
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    }
}

void Document::socketStateChanged(QAbstractSocket::SocketState state)
{
#warning "this isn't firing right now, so documents aren't populating for new users"
    qDebug() << state;
    if (state == QAbstractSocket::ConnectedState) {
        qDebug() << "Connection established.";
        for (int i = 0; i < clientList.size(); i++) {
            if (sender() == clientList.at(i)) {
                // sender() is the sender of the signal that calls this slot
                clientList.at(i)->write(QString("doc:%1,%2,%3:%4")
                                        .arg(0).arg(0).arg(editor->document()->characterCount()).arg(editor->toPlainText()).toAscii());
            }
        }
    }
}

void Document::disconnected()
{
    qDebug() << "Disconnected";
    participantPane->removeAllParticipants();

}






