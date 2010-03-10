#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QString>

static const int MaxBufferSize = 1024000;

class Client : public QObject
{
    Q_OBJECT

public:
    Client();

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

    void setGreetingMessage(const QString &message);
    void sendMessage(const QString &message);
    QString nickName() const;
    bool hasConnection(const QHostAddress &senderIp, int senderPort = -1) const;

    QTcpSocket *socket;

protected:
    void timerEvent(QTimerEvent *timerEvent);

signals:
    void newMessage(const QString &from, const QString &message);
    void newParticipant(const QString &nick);
    void participantLeft(const QString &nick);

private slots:
    void newConnection();
    void connectionError(QAbstractSocket::SocketError socketError);
    void disconnected();
    void readyForUse();

    void processReadyRead();
    void sendPing();
    void sendGreetingMessage();

private:
    void closeConnection();

    int readDataIntoBuffer(int maxSize = MaxBufferSize);
    int dataLengthForCurrentDataType();
    bool readProtocolHeader();
    bool hasEnoughData();
    void processData();

    QString greetingMessage;
    QString username;
    QTimer pingTimer;
    QTime pongTime;
    QByteArray buffer;
    ConnectionState state;
    DataType currentDataType;
    int numBytesForCurrentDataType;
    int transferTimerId;
    bool isGreetingMessageSent;


};

#endif // CLIENT_H
