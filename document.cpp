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
#include <QMessageBox>

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

    connect(participantPane, SIGNAL(memberCanNowRead(QTcpSocket*)), this, SLOT(populateDocumentForUser(QTcpSocket*)));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);
//    client = new Client();

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);


    myName = "Owner"; // temporary

    isAlreadyAnnounced = false;
    isOwner = true; // We are the document owner, unless we're connecting to someone elses document

    isFirstTime = true; // find utility to help wrap around the document
}

Document::~Document()
{
    delete ui;
}

void Document::connectToDocument(QStringList *list)
{
    isOwner = false;
    myName = list->at(0);
    QString address = list->at(1);
    QString portString = list->at(2);
    int port = portString.toInt();
    socket->connectToHost(QHostAddress(address), port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    participantPane->setOwnership(isOwner);
    isAlreadyAnnounced = true;
    setChatHidden(false);
    setParticipantsHidden(false);
    participantPane->setConnectInfo(QString("%1:%2").arg(address).arg(portString));
    delete list;
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void Document::undo()
{
    if (editor->hasFocus()) {
        editor->undo();
    }
    else if (chatPane->hasFocus()) {
        chatPane->undo();
    }
}

void Document::redo()
{
    if (editor->hasFocus()) {
        editor->redo();
    }
    else if (chatPane->hasFocus()) {
        chatPane->redo();
    }
}

void Document::cut()
{
    if (editor->hasFocus()) {
        editor->cut();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->cut();
    }
}

void Document::copy()
{
    if (editor->hasFocus()) {
        editor->copy();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->copy();
    }
}

void Document::paste()
{
    if (editor->hasFocus()) {
        editor->paste();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->paste();
    }
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
    isAlreadyAnnounced = true;
    participantPane->setOwnership(isOwner);
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

void Document::findAll(QString searchString, bool ignoreCase)
{
    QTextDocument *document = editor->document();

    bool found = false;

    if (isFirstTime == false) {
        document->undo();
    }

    if (searchString == "") {
        QMessageBox::information(this, tr("Empty Search Field"),
                "The search field is empty. Please enter a word and click Find.");
    }
    else {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::yellow);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();
        isFirstTime = false;

        if (found == false) {
            QMessageBox::information(this, tr("Word Not Found"),
                "Sorry, the word cannot be found.");
        }
    }

}

void Document::findNext(QString searchString, bool ignoreCase, bool wrapAround)
{
    (void)ignoreCase;
    (void)wrapAround;

    // findAll(searchString, ignoreCase);

    QTextDocument *document = editor->document();

    bool found = false;

    if (searchString == "") {
        // this shouldn't happen, ensure this in the find dialog
    }
    else {
        QTextCursor selectionCursor(document);
        selectionCursor.setPosition(editor->textCursor().position());

        bool firstLoop = true;

        selectionCursor = document->find(searchString, selectionCursor); //, QTextDocument::FindWholeWords);

        if (!selectionCursor.isNull()) {
            found = true;
            selectionCursor.movePosition(QTextCursor::EndOfWord, //WordRight,
                                         QTextCursor::KeepAnchor);
            editor->setTextCursor(selectionCursor);
        }
        else if (firstLoop) {
            firstLoop = false;
            selectionCursor.setPosition(0);
            selectionCursor = document->find(searchString, selectionCursor); //, QTextDocument::FindWholeWords);

            if (!selectionCursor.isNull()) {
                found = true;
                selectionCursor.movePosition(QTextCursor::EndOfWord, //WordRight,
                                             QTextCursor::KeepAnchor);
                editor->setTextCursor(selectionCursor);
            }
        }

        if (found == false) {
            emit notFound();
        }
    }
}

void Document::findPrev(QString searchString, bool ignoreCase, bool wrapAround)
{
    (void)ignoreCase;
    (void)wrapAround;

    QTextDocument *document = editor->document();

    bool found = false;
    bool firstLoop = true;

    if (searchString == "") {
        // this shouldn't happen, ensure this in the find dialog
    }
    else {
        QTextCursor selectionCursor(document);

        selectionCursor.setPosition(editor->textCursor().selectionStart()); // selectionStart returns the position OR the start of the cursor's selection's pos

        selectionCursor = document->find(searchString, selectionCursor, QTextDocument::FindBackward); //, QTextDocument::FindWholeWords);

        if (!selectionCursor.isNull()) {
            found = true;
            selectionCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            editor->setTextCursor(selectionCursor);
        }
        else if (firstLoop) {
            firstLoop = false;

            QTextCursor looparoundCursor(editor->document()->lastBlock());
            looparoundCursor.movePosition(QTextCursor::EndOfLine);
            
            looparoundCursor = document->find(searchString, looparoundCursor, QTextDocument::FindBackward); //, QTextDocument::FindWholeWords);

            if (!looparoundCursor.isNull()) {
                found = true;
                looparoundCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                editor->setTextCursor(looparoundCursor);
            }
        }
        if (!found) {
            emit notFound();
        }
    }
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

bool Document::isAnnounced()
{
    return isAlreadyAnnounced;
}

void Document::ownerIncomingData(QString data, QTcpSocket *sender)
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

    for (int i = 0; i < participantPane->participantList.size(); i++) {
        if (participantPane->participantList.at(i)->socket != sender &&
            participantPane->canRead(participantPane->participantList.at(i)->socket)) {
            participantPane->participantList.at(i)->socket->write(toSend.toAscii());
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
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            if (participantPane->canRead(participantPane->participantList.at(i)->socket)) {
                participantPane->participantList.at(i)->socket->write(toSend.toAscii());
            }
        }
    }
    else {
        socket->write(QString("doc:%1,%2,%3:%4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data).toAscii());
    }
}

void Document::onChatSend(QString str)
{
    if (isOwner) {
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            participantPane->participantList.at(i)->socket->write(QString(myName + ": ").toAscii() + str.toAscii());
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
        QTcpSocket *sock = qobject_cast<QTcpSocket *>(sender());
        data = sock->readAll();
        if (participantPane->canWrite(sock)) {
            ownerIncomingData(data, sock);
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
        participantPane->insertParticipant("Newbie", server->nextPendingConnection());
        connect(participantPane->participantList.last()->socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        connect(participantPane->participantList.last()->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    }
}

void Document::populateDocumentForUser(QTcpSocket *socket)
{
    qDebug() << "Sending entire document";
    socket->write(QString("doc:%1,%2,%3:%4").arg(0).arg(0).arg(editor->document()->characterCount()).arg(editor->toPlainText()).toAscii());
}

void Document::disconnected()
{
    if (isOwner) {
        QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
        participantPane->removeParticipant(socket);
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            if (socket == participantPane->participantList.at(i)->socket) {
                participantPane->participantList.removeAt(i);
                return;
            }
        }
    }
    else {
        participantPane->removeAllParticipants();
        chatPane->hide();
        participantPane->hide();
    }
}






