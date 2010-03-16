#include "document.h"
#include "ui_document.h"

#include "utilities.h"

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
#include <QStringList>

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
    editor->setFont(QFont(Utilities::codeFont, Utilities::codeFontSize));
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, editor);
    
    // for split pane editing
    delete ui->bottomEditorFrame;
    bottomEditor = new CodeEditor(this);
    bottomEditor->setFont(editor->font());
    bottomEditor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(1, bottomEditor);
    // we don't need the default document since we're using the document of the original editor
    bottomEditor->document()->deleteLater();
    bottomEditor->setDocument(editor->document());
    bottomEditor->hide();

    highlighter = new CppHighlighter(editor->document());

    delete ui->participantFrame;
    participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, participantPane);

    delete ui->chatFrame;
    chatPane = new ChatPane();
    ui->codeChatSplitter->insertWidget(1, chatPane);

    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);
//    client = new Client();

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    myName = "Owner"; // temporary

    isAlreadyAnnounced = false;
    isOwner = true; // We are the document owner, unless we're connecting to someone elses document
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
    participantPane->setOwnership(isOwner);
    isAlreadyAnnounced = true;
    setChatHidden(false);
    setParticipantsHidden(false);
    participantPane->setConnectInfo(QString("%1:%2").arg(address).arg(portString));
    delete list;

    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(chatPane, SIGNAL(returnPressed(QString)), this, SLOT(onChatSend(QString)));

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
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
        editor->setFocus();
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

void Document::setHighlighter(int Highlighter)
{
    if (Highlighter == None) {
        highlighter->deleteLater();
    }
    else if (Highlighter == CPlusPlus) {

    }
    else if (Highlighter == Python) {

    }
}

void Document::announceDocument()
{
    isAlreadyAnnounced = true;
    participantPane->setOwnership(isOwner);
    setChatHidden(false);
    setParticipantsHidden(false);

    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(chatPane, SIGNAL(returnPressed(QString)), this, SLOT(onChatSend(QString)));

    connect(participantPane, SIGNAL(memberCanNowRead(QTcpSocket*)), this, SLOT(populateDocumentForUser(QTcpSocket*)));

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    server->listen(QHostAddress::Any, 0); // Port is chosen automatically, listening on all NICs
    QString port = QString::number(server->serverPort(), 10);

    chatPane->appendChatMessage("Listening on port " + port);

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address())
            ipAddress = ipAddressesList.at(i).toString();
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty()) {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    participantPane->setConnectInfo(ipAddress + ":" + port);
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
    editor->findAll(searchString, ignoreCase);
}

void Document::findNext(QString searchString, bool ignoreCase, bool wrapAround)
{
    if (editor->findNext(searchString, ignoreCase, wrapAround)) {
        emit notFound();
    }
}

void Document::findPrev(QString searchString, bool ignoreCase, bool wrapAround)
{
    if (editor->findPrev(searchString, ignoreCase, wrapAround)) {
        emit notFound();
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
    bottomEditor->show();
}

void Document::unSplitEditor()
{
    if (!isEditorSplit()) {
        return;
    }
    bottomEditor->hide();
}

bool Document::isEditorSplit()
{
    return !bottomEditor->isHidden();
}

bool Document::isAnnounced()
{
    return isAlreadyAnnounced;
}

void Document::ownerIncomingData(QString data, QTcpSocket *sender, int length)
{
    qDebug() << "odata: " << data;
    QString toSend;

    bool haveExtra = false;
    QString rest;

    if (length < data.length()) { // in case incoming data packets were concatenated
        haveExtra = true;
        rest = data;
        rest.remove(0, length);
        data.remove(length, data.length() - length);
    }
    else if (length > data.length()) {
        // we have incomplete data
    }

    QRegExp rx;
    if (data.startsWith("doc:")) {
        toSend = data;
        data.remove(0, 4);
        // detect line number, then put text at that line.
        rx = QRegExp("(\\d+)\\s(\\d+)\\s(\\d+)\\s(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            data = rx.cap(4);
            editor->collabTextChange(pos, charsRemoved, charsAdded, data);
        }
    }
    else if (data.startsWith("helo:")) {
        qDebug() << "helo received";
        data.remove(0, 5);
        rx = QRegExp("([a-zA-Z0-9_]*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            participantPane->updateName(name, socket);
            toSend = "join:" + name;
        }
    }
    else if (data.startsWith("resync")) { // user requesting resync of the entire document
        populateDocumentForUser(sender);
    }
    else {
        toSend = QString("Someone: %2").arg(data);
        chatPane->appendChatMessage(toSend);
    }

    // Distribute data to all the other participants
    for (int i = 0; i < participantPane->participantList.size(); i++) {
        if (participantPane->participantList.at(i)->socket != sender &&
            participantPane->canRead(participantPane->participantList.at(i)->socket)) {
            participantPane->participantList.at(i)->socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
        }
    }

    bool ok;

    if (haveExtra) {
        QRegExp rx = QRegExp("^(\\d+)*.");
        if (rest.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            rest.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok) {
                ownerIncomingData(rest, sender, length);
            }
        }
    }

}

void Document::participantIncomingData(QString data, int length)
{
    bool haveExtra = false;
    QString rest;

    if (length < data.length()) { // in case incoming data packets were concatenated
        haveExtra = true;
        rest = data;
        rest.remove(0, length);
        data.remove(length, data.length() - length);
    }
    else if (length > data.length()) {
        // we have incomplete data
    }

    qDebug() << "pdata: " << data;

    QRegExp rx;
    if (data.startsWith("doc:")) {
        data.remove(0, 4);
        // detect line number, then put text at that position.
        rx = QRegExp("(\\d+)\\s(\\d+)\\s(\\d+)\\s(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            data = rx.cap(4);
            editor->collabTextChange(pos, charsRemoved, charsAdded, data);
        }
    }
    else if (data.startsWith("join:")) {
        data.remove(0, 5);
        rx = QRegExp("([a-zA-Z0-9_]*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            participantPane->newParticipant(name);
        }
    }
    else if (data.startsWith("leave:")) {
        data.remove(0, 6);
        rx = QRegExp("([a-zA-Z0-9_]*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            participantPane->removeParticipant(name);
        }
    }
    else if (data.startsWith("populate:")) { // populate users data

    }
    else if (data.startsWith("sync:")) { // this packet is the entire document
        data.remove(0, 5);
        // set the document's contents to the contents of the packet
        editor->setPlainText(data);
    }
    else if (data.startsWith("promote:")) {

    }
    else if (data.startsWith("demote:")) {

    }
    else {
        chatPane->appendChatMessage(data);
    }

    bool ok;

    if (haveExtra) {
        QRegExp rx = QRegExp("^(\\d+)*.");
        if (rest.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            rest.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok) {
                participantIncomingData(rest, length);
            }
        }
    }
}

void Document::onTextChange(int pos, int charsRemoved, int charsAdded)
{
    QString toSend;

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
        toSend = QString("doc:%1 %2 %3 %4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);
        qDebug() << "toSend: " << toSend;
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            if (participantPane->canRead(participantPane->participantList.at(i)->socket)) {
                participantPane->participantList.at(i)->socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
            }
        }
    }
    else {
        toSend = QString("doc:%1 %2 %3 %4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);
        socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
    }
}

void Document::onChatSend(QString str)
{
    QString toSend;

    if (isOwner) {
        toSend = QString("%1: %2").arg(myName).arg(str);
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            participantPane->participantList.at(i)->socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
        }
    }
    else {
        toSend = str; // for find/replace, much of this will be rewritten later for better Object Oriented design
        socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
    }
    chatPane->appendChatMessage(QString("%1: %2").arg(myName).arg(str));
}

void Document::onIncomingData()
{
    // disconnect the signal that fires when the contents of the editor change so we don't echo
    disconnect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));

    QString data;
    QRegExp rx = QRegExp("^(\\d+)*.");
    int length;
    bool ok;

    if (isOwner) {
        // cast the sender() of this signal to a QTcpSocket
        QTcpSocket *sock = qobject_cast<QTcpSocket *>(sender());
        data = sock->readAll();
        if (data.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            data.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok && participantPane->canWrite(sock)) {
                ownerIncomingData(data, sock, length);
            }
        }
    }
    else { // We are a participant
        data = socket->readAll();
        if (data.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            data.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok) {
                participantIncomingData(data, length);
            }
        }
    }
    // reconnect the signal that fires when the contents of the editor change so we continue to send new text
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
}

void Document::onNewConnection()
{
    if (isOwner) {
        participantPane->newParticipant(server->nextPendingConnection());
        connect(participantPane->participantList.last()->socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        connect(participantPane->participantList.last()->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    }
    else {
        qDebug() << "helo:" << myName;
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        // The owner does not get this message, probably because it comes before the readyRead() is hooked up?
        socket->write(QString("helo:%1").arg(myName).toAscii());
    }
}

void Document::populateDocumentForUser(QTcpSocket *socket)
{
    qDebug() << "Sending entire document";
    QString toSend = QString("sync:%1").arg(editor->toPlainText());
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
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






