#include "server.h"

#include <QtNetwork>

static const int TransferTimeout = 30 * 1000;
static const int PongTimeout = 60 * 1000;
static const int PingInterval = 5 * 1000;
static const char SeparatorToken = ' ';

Server::Server(QObject *parent) :
    QTcpServer(parent)
{

}

void Server::newConnection(QTcpSocket *socket)
{
    clientList.append(socket);
}