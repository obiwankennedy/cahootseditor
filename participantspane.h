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

    // Owner participant pane function
    void newParticipant(QTcpSocket *socket);

    // Client participant pane add participant functions
    void newParticipant(QString name);

    // Owner participant function to update someone's name when it gets populated
    void updateName(QString name, QTcpSocket *socket);

    // For when we disconnect
    void removeAllParticipants();
    // This is a function to be used by the owner
    void removeParticipant(QTcpSocket *socket);
    // This is a function to be used by the participants in removing participants via control messages.
    void removeParticipant(QString name);

    bool canWrite(QTcpSocket *socket);
    bool canRead(QTcpSocket *socket);

    QList<Participant*> participantList;
    QMap<QTcpSocket *, Participant *> participantMap;

private:
    Ui::ParticipantsPane *ui;

    enum Row {
        ReadWrite = 2,
        ReadOnly = 1,
        Waiting = 0,

        Owner = 1,
    };

    QTreeWidgetItem *rwItem;
    QTreeWidgetItem *roItem;
    QTreeWidgetItem *waitItem;
    QTreeWidgetItem *owner;

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

signals:
    void memberCanNowRead(QTcpSocket *);
};

#endif // PARTICIPANTSPANE_H
