#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTime>
#include <QTimer>

static const int MaxBufferSize = 1024000;

class Server : public QTcpServer
{
Q_OBJECT
public:
    explicit Server(QObject *parent = 0);

    enum ConnectionState {
        WaitingForGreeting,
        ReadingGreeting,
        ReadyForUse
    };
    enum DataType {
        PlainText,
        Ping,
        Pong,
        Greeting,
        ChatMsg,
        UserJoinLeave,
        Undefined
    };

    bool readProtocolHeader();

signals:

public slots:

private:
    QTimer pingTimer;
    QTime pongTime;
    QByteArray buffer;
    ConnectionState state;

};

#endif // SERVER_H
