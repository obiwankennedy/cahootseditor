#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QHostAddress>
#include <QTcpSocket>

namespace Ui {
    class ParticipantsPane;
}

struct Participant {
    QString name;
    QHostAddress address;
    QColor color;
    QTreeWidgetItem *item;
    int permissions;
    QTcpSocket *socket;
};

class ParticipantsPane : public QWidget {
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setOwnership(bool isOwner);

    void setConnectInfo(QString str);

    // Participants are first new'd (prior to greetings)
    void newParticipant(QTcpSocket *socket);
    // ...and later added when they send their greetings
    bool addParticipant(QString name, QTcpSocket *socket);
    // getName allows us to set their name in the chat pane correctly
    QString getNameForSocket(QTcpSocket *socket);

    // Client participant pane add participant functions
    void newParticipant(QString name);

    // For when we disconnect
    void removeAllParticipants();
    // This is a function to be used by the owner
    void removeParticipant(QTcpSocket *socket);
    // This is a function to be used by the participants in removing participants via control messages.
    void removeParticipant(QString name);

    void promoteParticipant(QString name, QString address);
    void demoteParticipant(QString name, QString address);

    bool canWrite(QTcpSocket *socket);
    bool canRead(QTcpSocket *socket);

    QList<Participant*> participantList;
    QMap<QTcpSocket *, Participant *> participantMap;

private:
    Ui::ParticipantsPane *ui;

    QTreeWidgetItem *rwItem;
    QTreeWidgetItem *roItem;
    QTreeWidgetItem *waitItem;
    QTreeWidgetItem *owner;

    bool isOwner;

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

signals:
    void memberCanNowRead(QTcpSocket *);
    void promoteClicked(QString participant);
    void demoteClicked(QString participant);
};

#endif // PARTICIPANTSPANE_H
