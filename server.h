#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "participantspane.h"
#include "codeeditor.h"
#include "chatpane.h"

class Server : public QObject
{
Q_OBJECT
public:
    explicit Server(CodeEditor *editor, ParticipantsPane *participantsPane, ChatPane *chatPane, QObject *parent = 0);

    bool listen(const QHostAddress & address = QHostAddress::Any, quint16 port = 0);

    quint16 serverPort();

    // This writes to all sockets except exception
    void writeToAll(QString data, QTcpSocket *exception = 0);

private:
    CodeEditor *editor;
    ParticipantsPane *participantPane;
    ChatPane *chatPane;

    QTcpServer *server;

    void processData(QString data, QTcpSocket *sender, int length = 0);

signals:

public slots:

private slots:
    void onTextChange(int pos, int charsRemoved, int charsAdded);

    void onChatSend(QString str);
    void onIncomingData();
    void onNewConnection();

    void memberPermissionsChanged(QTcpSocket *participant, QString permissions, bool wasPromoted);

    void populateDocumentForUser(QTcpSocket *socket);

    void disconnected();
};

#endif // SERVER_H
