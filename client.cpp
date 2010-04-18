/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "client.h"

#include <QTextDocumentFragment>
#include <QMessageBox>

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
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    permissions = Enu::Waiting;

    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    blockSize = 0;
}

void Client::connectToHost(QHostAddress hostName, int port)
{
    socket->connectToHost(hostName, port);
}

void Client::writeToServer(QString string)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    // Reserve space for a 16 bit int that will contain the total size of the block we're sending
    out << (quint32)0;

    // Write the data to the stream
    out << string;

    // Move the head to the beginning and replace the reserved space at the beginning with the size of the block.
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    socket->write(block);
}

void Client::setUsername(QString username)
{
    myName = username;
}

void Client::resynchronize()
{
    QString toSend;

    toSend = "resync";
    writeToServer(toSend);
    participantPane->removeAllParticipants();
    editor->setReadOnly(true);
}

void Client::processData(QString data)
{
//    qDebug() << "pdata: " << data;

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
            editor->setReadOnly(false);
            participantPane->setDisabled(false);
        }
        else if (data == "read") {
            permissions = Enu::ReadOnly;
            editor->setReadOnly(true);
            participantPane->setDisabled(false);
        }
        else if (data == "waiting") {
            permissions = Enu::Waiting;
            editor->setReadOnly(true);
            participantPane->setDisabled(true);
//            participantPane->removeAllParticipants();
        }
    }
    else if (data.startsWith("adduser:")) {
        data.remove(0, 8);
//        qDebug() << "add user: " << data;
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
    writeToServer(toSend);
}

void Client::onChatSend(QString str)
{
    QString toSend;

    toSend = "chat:" + str;
    writeToServer(toSend);

    chatPane->appendChatMessage(QString("%1:\t%2").arg(myName).arg(str));
}

void Client::onIncomingData()
{
    // disconnect the signal that fires when the contents of the editor change so we don't echo
    disconnect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));

    while (socket->bytesAvailable() > 0) { // process data until we run out, or have an incomplete packet - the returns will handle that case
        QString data;
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_4_6);

        if (blockSize == 0) { // blockSize is 0, so we don't know how big the next packet is yet
            // We check if we have at least the size of quint32 to read, if not, return and wait for the next readyRead
            if (socket->bytesAvailable() < (int)sizeof(quint32))
                return;

            // blockSize is the first chunk, so fetch that to find out how big this packet is going to be
            in >> blockSize;
        }

        // If we don't yet have the full size of the packet, return and wait for the rest
        if (socket->bytesAvailable() < blockSize) {
            return;
        }

        in >> data;

        processData(data);

        blockSize = 0; // reset blockSize to 0 for the next packet
    }

    // reconnect the signal that fires when the contents of the editor change so we continue to send new text
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
}

void Client::onNewConnection()
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    QString toSend = QString("helo:%1").arg(myName);
    writeToServer(toSend);
}

void Client::disconnected()
{
    chatPane->appendChatMessage("Disconnected.", Qt::red);
    chatPane->setReadOnly(true);
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(editor, tr("Cahoots"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(editor, tr("Cahoots"),
                                 tr("The connection was refused. "
                                    "Make sure the server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(editor, tr("Cahoots"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }
}
