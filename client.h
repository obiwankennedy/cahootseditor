#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "codeeditor.h"
#include "participantspane.h"
#include "chatpane.h"

#include <QHostAddress>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(CodeEditor *editor, ParticipantsPane *participantsPane, ChatPane *chatPane, QObject *parent = 0);

    void connectToHost(QHostAddress hostName, int port);

    void writeToServer(QString string);

    void setUsername(QString username);

    void resynchronize();

private:
    CodeEditor *editor;
    ParticipantsPane *participantPane;
    ChatPane *chatPane;

    QTcpSocket *socket;

    QString myName;
    Enu::Permissions permissions;

    void processData(QString data);

    quint32 blockSize;

private slots:
    void onTextChange(int pos, int charsRemoved, int charsAdded);

    void onChatSend(QString str);
    void onIncomingData();
    void onNewConnection();

    void disconnected();

    void displayError(QAbstractSocket::SocketError socketError);
};

#endif // CLIENT_H
