#include "client.h"

#include <QtNetwork>

static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 60 * 1000;
static const int PingInterval = 5 * 1000;
static const char SeparatorToken = ' ';

Client::Client()
{    
    greetingMessage = tr("undefined");
    username = tr("unknown");
    state = WaitingForGreeting;
    currentDataType = Undefined;
    numBytesForCurrentDataType = -1;
    transferTimerId = 0;
    isGreetingMessageSent = false;
    pingTimer.setInterval(PingInterval);

    connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(socket, SIGNAL(disconnected()), &pingTimer, SLOT(stop()));
    connect(&pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    connect(socket, SIGNAL(connected()), this, SLOT(sendGreetingMessage()));
}

void Client::setGreetingMessage(const QString &message)
{
    greetingMessage = message;
}

void Client::sendMessage(const QString &message)
{
    if (message.isEmpty()) {
        return;
    }

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + " " + msg;
    if (socket->write(data) == data.size()) {
        // potential debug
    }
}

QString Client::nickName() const
{
    return QString(nickName() + "@" + QHostInfo::localHostName() + ":" + QString::number(socket->peerPort()));
}

bool Client::hasConnection(const QHostAddress &senderIp, int senderPort) const
{
    if (socket->peerPort() == senderPort) {
        return true;
    }


    return false;
}

void Client::timerEvent(QTimerEvent *timerEvent)
{
    if (timerEvent->timerId() == transferTimerId) {
        abort();
        killTimer(transferTimerId);
        transferTimerId = 0;
    }
}

void Client::newConnection()
{
    QByteArray greeting = greetingMessage.toUtf8();
    QByteArray data = "GREETING " + QByteArray::number(greeting.size()) + " " + greeting;
    if (socket->write(data) == data.size())
        isGreetingMessageSent = true;

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void Client::connectionError(QAbstractSocket::SocketError /* socketError */)
{
    closeConnection();
}

void Client::disconnected()
{
    closeConnection();
}

void Client::readyForUse()
{
    if (!socket || (socket->state() != QTcpSocket::ConnectedState)) {
        return;
    }

    connect(socket, SIGNAL(newMessage(const QString &, const QString &)),
            this, SIGNAL(newMessage(const QString &, const QString &)));
}

void Client::processReadyRead()
{
    if (state == WaitingForGreeting) {
        if (!readProtocolHeader())
            return;
        if (currentDataType != Greeting) {
            socket->abort();
            return;
        }
        state = ReadingGreeting;
    }

    if (state == ReadingGreeting) {
        if (!hasEnoughData())
            return;

        buffer = socket->read(numBytesForCurrentDataType);
        if (buffer.size() != numBytesForCurrentDataType) {
            socket->abort();
            return;
        }

        username = QString(buffer) + "@" + socket->peerAddress().toString() + ":"
                   + QString::number(socket->peerPort());
        currentDataType = Undefined;
        numBytesForCurrentDataType = 0;
        buffer.clear();

        if (!socket->isValid()) {
            socket->abort();
            return;
        }

        if (!isGreetingMessageSent)
            sendGreetingMessage();

        pingTimer.start();
        pongTime.start();
        state = ReadyForUse;
        readyForUse();
    }

    do {
        if (currentDataType == Undefined) {
            if (!readProtocolHeader())
                return;
        }
        if (!hasEnoughData())
            return;
        processData();
    } while (socket->bytesAvailable() > 0);
}

void Client::sendPing()
{
    if (pongTime.elapsed() > PongTimeout) {
        socket->abort();
        return;
    }

    socket->write("PING 1 p");
}

void Client::sendGreetingMessage()
{
    QByteArray greeting = greetingMessage.toUtf8();
    QByteArray data = "GREETING " + QByteArray::number(greeting.size()) + " " + greeting;
    if (socket->write(data) == data.size())
        isGreetingMessageSent = true;
}

void Client::closeConnection()
{
    socket->deleteLater();
}

int Client::readDataIntoBuffer(int maxSize)
{
    if (maxSize > MaxBufferSize)
        return 0;

    int numBytesBeforeRead = buffer.size();
    if (numBytesBeforeRead == MaxBufferSize) {
        socket->abort();
        return 0;
    }

    while (socket->bytesAvailable() > 0 && buffer.size() < maxSize) {
        buffer.append(socket->read(1));
        if (buffer.endsWith(SeparatorToken))
            break;
    }
    return buffer.size() - numBytesBeforeRead;
}

int Client::dataLengthForCurrentDataType()
{
    if (socket->bytesAvailable() <= 0 || readDataIntoBuffer() <= 0
            || !buffer.endsWith(SeparatorToken))
        return 0;

    buffer.chop(1);
    int number = buffer.toInt();
    buffer.clear();
    return number;
}

bool Client::readProtocolHeader()
{
    if (transferTimerId) {
        socket->killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (readDataIntoBuffer() <= 0) {
        transferTimerId = socket->startTimer(TransferTimeout);
        return false;
    }

    if (buffer == "PING ") {
        currentDataType = Ping;
    } else if (buffer == "PONG ") {
        currentDataType = Pong;
    } else if (buffer == "MESSAGE ") {
        currentDataType = PlainText;
    } else if (buffer == "GREETING ") {
        currentDataType = Greeting;
    } else {
        currentDataType = Undefined;
        abort();
        return false;
    }

    buffer.clear();
    numBytesForCurrentDataType = dataLengthForCurrentDataType();
    return true;
}

bool Client::hasEnoughData()
{
    if (transferTimerId) {
        socket->killTimer(transferTimerId);
        transferTimerId = 0;
    }

    if (numBytesForCurrentDataType <= 0)
        numBytesForCurrentDataType = dataLengthForCurrentDataType();

    if (socket->bytesAvailable() < numBytesForCurrentDataType
            || numBytesForCurrentDataType <= 0) {
        transferTimerId = startTimer(TransferTimeout);
        return false;
    }

    return true;
}

void Client::processData()
{
    buffer = socket->read(numBytesForCurrentDataType);
    if (buffer.size() != numBytesForCurrentDataType) {
        abort();
        return;
    }

    switch (currentDataType) {
    case PlainText:
        newMessage(username, QString::fromUtf8(buffer));
        break;
    case Ping:
        socket->write("PONG 1 p");
        break;
    case Pong:
        pongTime.restart();
        break;
    default:
        break;
    }

    currentDataType = Undefined;
    numBytesForCurrentDataType = 0;
    buffer.clear();
}



