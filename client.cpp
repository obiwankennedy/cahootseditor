#include "client.h"

#include <QTextDocumentFragment>

Client::Client(CodeEditor *editor, ParticipantsPane *participantsPane, ChatPane *chatPane, QObject *parent) :
    QObject(parent)
{
    this->editor = editor;
    this->participantPane = participantsPane;
    this->chatPane = chatPane;

    socket = new QTcpSocket(this);

    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(chatPane, SIGNAL(returnPressed(QString)), this, SLOT(onChatSend(QString)));

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(connected()), this, SLOT(onNewConnection()));

    permissions = Enu::Waiting;

    socket->setSocketOption(QAbstractSocket::KeepAliveOption);
    blockSize = 0;
}

void Client::connectToHost(QHostAddress hostName, int port)
{
    socket->connectToHost(hostName, port);
}

void Client::setUsername(QString username)
{
    myName = username;
}

void Client::resynchronize()
{
    QString toSend;

    toSend = "resync";
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
    participantPane->removeAllParticipants();
    editor->setReadOnly(true);
}

void Client::processData(QString data, int length)
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
    else if (data.startsWith("chat:")) {
        data.remove(0, 5);
        chatPane->appendChatMessage(data);
    }
    else if (data.startsWith("setperm:")) {
        data.remove(0, 8);
        rx = QRegExp("([a-zA-Z0-9_]*)@([0-9\\.]*):(waiting|read|write)");
        QString name;
        QString address;
        QString permissions;
        if (data.contains(rx)) {
            name = rx.cap(1);
            address = rx.cap(2);
            permissions = rx.cap(3);
            participantPane->setParticipantPermissions(name, address, permissions);
        }
    }
    else if (data.startsWith("join:")) {
        data.remove(0, 5);
        rx = QRegExp("([a-zA-Z0-9_]*)@(.*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            QString address = rx.cap(2);
            participantPane->newParticipant(name, address);
        }
    }
    else if (data.startsWith("leave:")) {
        data.remove(0, 6);
        rx = QRegExp("([a-zA-Z0-9_]*)@(.*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            QString address = rx.cap(2);
            participantPane->removeParticipant(name, address);
        }
    }
    else if (data.startsWith("updateperm:")) { // the server has updated our permissions
        data.remove(0, 11);
        if (data == "write") {
            permissions = Enu::ReadWrite;
            editor->setDisabled(false);
            editor->setReadOnly(false);
        }
        else if (data == "read") {
            permissions = Enu::ReadOnly;
            editor->setDisabled(false);
            editor->setReadOnly(true);
        }
        else if (data == "waiting") {
            permissions = Enu::Waiting;
            editor->setDisabled(true);
            editor->setReadOnly(true);
            participantPane->removeAllParticipants();
        }
    }
    else if (data.startsWith("adduser:")) {
        data.remove(0, 8);
        qDebug() << "add user: " << data;
        rx = QRegExp("([a-zA-Z0-9_]*)@([0-9\\.]*):(waiting|read|write)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            QString address = rx.cap(2);
            QString permissions = rx.cap(3);
            participantPane->newParticipant(name, address, permissions);
        }
    }
    else if (data.startsWith("sync:")) { // the data is the entire document
        data.remove(0, 5);
        // set the document's contents to the contents of the packet
        editor->setPlainText(data);
        editor->setReadOnly(Enu::ReadWrite != permissions);
    }
    else if (data.startsWith("helo:")) {
        data.remove(0, 5);
        participantPane->setOwnerName(data);
    }

    bool ok;

    if (haveExtra) {
        QRegExp rx = QRegExp("^(\\d+)*.");
        if (rest.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            rest.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok) {
                processData(rest, length);
            }
        }
    }
}

void Client::onTextChange(int pos, int charsRemoved, int charsAdded)
{
    QString toSend;

    if (socket->state() != QAbstractSocket::ConnectedState) {
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

    toSend = QString("doc:%1 %2 %3 %4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
}

void Client::onChatSend(QString str)
{
    QString toSend;

    toSend = "chat:" + str;
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());

    chatPane->appendChatMessage(QString("%1:\t%2").arg(myName).arg(str));
}

void Client::onIncomingData()
{
    // disconnect the signal that fires when the contents of the editor change so we don't echo
    disconnect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));

    QString data;
    QRegExp rx = QRegExp("^(\\d+)*.");
    int length;
    bool ok;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_6);

//    if (blockSize == 0) { // blocksize is 0, so we don't know how big the next packet is yet
//        if (socket->bytesAvailable() < (int)sizeof(quint16))
//            return;
//
//        in >> blockSize; // blockSize is the first chunk, so fetch that to find out how big this packet is
//    }
//
//    if (tcpSocket->bytesAvailable() < blockSize) {
//        return;
//    }

    blockSize = 0; // reset blockSize to 0 for the next packet

    // We are a participant
    data = socket->readAll();
    if (data.contains(rx)) {
        length = rx.cap(1).toInt(&ok);
        data.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
        if (ok) {
            processData(data, length);
        }
    }
    // reconnect the signal that fires when the contents of the editor change so we continue to send new text
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
}

void Client::onNewConnection()
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    QString toSend = QString("helo:%1").arg(myName);
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
}

void Client::disconnected()
{
    participantPane->removeAllParticipants();
    chatPane->hide();
    participantPane->hide();
}
