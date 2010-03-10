#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

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


signals:

public slots:

};

#endif // SERVER_H
