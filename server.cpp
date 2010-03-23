#include "server.h"

#include <QTextDocumentFragment>

Server::Server(CodeEditor *editor, ParticipantsPane *participantsPane, ChatPane *chatPane, QObject *parent) :
    QObject(parent)
{
    this->editor = editor;
    this->participantPane = participantsPane;
    this->chatPane = chatPane;

    server = new QTcpServer(this);

    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(chatPane, SIGNAL(returnPressed(QString)), this, SLOT(onChatSend(QString)));

    connect(participantPane, SIGNAL(memberCanNowRead(QTcpSocket*)), this, SLOT(populateDocumentForUser(QTcpSocket*)));
    connect(participantPane, SIGNAL(memberPermissionsChanged(QTcpSocket*,QString,bool)), this, SLOT(memberPermissionsChanged(QTcpSocket*,QString,bool)));

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

bool Server::listen(const QHostAddress &address, quint16 port)
{
    return server->listen(address, port);
}

quint16 Server::serverPort()
{
    return server->serverPort();
}

void Server::writeToAll(QString data, QTcpSocket *exception)
{
    qDebug() << "toSend: " << data;

    if (data != "") {
        for (int i = 0; i < participantPane->participantList.size(); i++) {
            if (participantPane->participantList.at(i)->socket != exception && participantPane->canRead(participantPane->participantList.at(i)->socket)) {
                participantPane->participantList.at(i)->socket->write(QString("%1 %2").arg(data.length()).arg(data).toAscii());
            }
        }
    }
}

void Server::processData(QString data, QTcpSocket *sender, int length)
{
    qDebug() << "odata: " << data;
    QString toSend;
    QTcpSocket *exception = 0;
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
        if (!participantPane->canWrite(sender)) {
            return;
        }
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
    else if (data.startsWith("chat:")) {
        if (!participantPane->canRead(sender)) {
            return;
        }
        data.remove(0, 5);
        toSend = QString("%1:\t%2").arg(participantPane->getNameForSocket(sender)).arg(data);
        chatPane->appendChatMessage(toSend);
        toSend.insert(0, "chat:");
        exception = sender;
    }
    else if (data.startsWith("resync")) { // user requesting resync of the entire document
        if (!participantPane->canRead(sender)) {
            return;
        }
        populateDocumentForUser(sender);
    }
    else if (data.startsWith("helo:")) {
        data.remove(0, 5);
        rx = QRegExp("([a-zA-Z0-9_]*)");
        if (data.contains(rx)) {
            QString name = rx.cap(1);
            if (participantPane->addParticipant(name, sender)) {
                toSend = "join:" + participantPane->getNameAddressForSocket(sender);
            }
            else {
                return;
            }
        }
    }

    // Distribute data to all the other participants
    writeToAll(toSend, exception);

    bool ok;

    if (haveExtra) {
        QRegExp rx = QRegExp("^(\\d+)*.");
        if (rest.contains(rx)) {
            length = rx.cap(1).toInt(&ok);
            rest.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
            if (ok) {
                processData(rest, sender, length);
            }
        }
    }
}

void Server::onTextChange(int pos, int charsRemoved, int charsAdded)
{
    QString toSend;
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
    writeToAll(toSend);
}

void Server::onChatSend(QString str)
{
    QString toSend;

#warning "Need a way to set the name for the document owner"
    QString myName = "Owner";

    toSend = QString("chat:%1:\t%2").arg(myName).arg(str);
    writeToAll(toSend);
    chatPane->appendChatMessage(QString("%1:\t%2").arg(myName).arg(str));
}

void Server::onIncomingData()
{
    // disconnect the signal that fires when the contents of the editor change so we don't echo
    disconnect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));

    QString data;
    QRegExp rx = QRegExp("^(\\d+)*.");
    int length;
    bool ok;

    // cast the sender() of this signal to a QTcpSocket
    QTcpSocket *sock = qobject_cast<QTcpSocket *>(sender());
    data = sock->readAll();
    if (data.contains(rx)) {
        length = rx.cap(1).toInt(&ok);
        data.remove(0, rx.cap(1).length() + 1); // remove digit indicating packet length and whitespace
        if (ok) {
            processData(data, sock, length);
        }
    }

    // reconnect the signal that fires when the contents of the editor change so we continue to send new text
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));

}

void Server::onNewConnection()
{
    QTcpSocket *sock = server->nextPendingConnection();
    connect(sock, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnected()));
    participantPane->newParticipant(sock);
}

void Server::memberPermissionsChanged(QTcpSocket *participant, QString permissions, bool wasPromoted)
{
    QString toSend = QString("setperm:%1").arg(permissions);
    participant->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());

    toSend = QString("%1:%2").arg(wasPromoted ? "promote:" : "demote:").arg(participantPane->getNameAddressForSocket(participant));

    for (int i = 0; i < participantPane->participantList.size(); i++) {
        if (participantPane->canRead(participantPane->participantList.at(i)->socket)) {
            participantPane->participantList.at(i)->socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
        }
    }
}

void Server::populateDocumentForUser(QTcpSocket *socket)
{
    qDebug() << "Sending entire document";
    QString toSend = QString("sync:%1").arg(editor->toPlainText());
    socket->write(QString("%1 %2").arg(toSend.length()).arg(toSend).toAscii());
}

void Server::disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    participantPane->removeParticipant(socket);
    for (int i = 0; i < participantPane->participantList.size(); i++) {
        if (socket == participantPane->participantList.at(i)->socket) {
            participantPane->participantList.removeAt(i);
            return;
        }
    }
}

