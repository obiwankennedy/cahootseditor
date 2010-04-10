#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>
#include "participantspane.h"
#include "codeeditor.h"
#include "chatpane.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(CodeEditor *editor, ParticipantsPane *participantsPane, ChatPane *chatPane, QObject *parent = 0);

    bool listen(const QHostAddress & address = QHostAddress::Any, quint16 port = 0);

    void startBroadcasting();

    quint16 serverPort();

    // This writes to all sockets except exception
    void writeToAll(QString string, QTcpSocket *exception = 0);

    void writeToSocket(QString string, QTcpSocket *socket);

    void resynchronize();

    void setOwnerName(QString name);

private:
    CodeEditor *editor;
    ParticipantsPane *participantPane;
    ChatPane *chatPane;

    QTcpServer *server;

    QUdpSocket *udpSocket;
    QTimer *timer;

    QString myName;

    void processData(QString data, QTcpSocket *sender);

private slots:
    void onTextChange(int pos, int charsRemoved, int charsAdded);

    void onChatSend(QString str);
    void onIncomingData();
    void onNewConnection();

    void memberPermissionsChanged(QTcpSocket *participant, QString permissions);

    void populateDocumentForUser(QTcpSocket *socket);

    void disconnected();

    void broadcastDatagram();

    void displayError(QAbstractSocket::SocketError socketError);
};

#endif // SERVER_H
