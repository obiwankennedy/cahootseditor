#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QHostAddress>

namespace Ui {
    class ParticipantsPane;
}

struct Participant {
    QString name;
    QHostAddress address;
    QColor color;
};

class ParticipantsPane : public QWidget {
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setConnectInfo(QString str);

    void insertParticipant(QString name);

    // For when we disconnect
    void removeAllParticipants();

    QList<Participant*> participantList;

private:
    Ui::ParticipantsPane *ui;

    enum Row {
        ReadWrite = 0,
        ReadOnly = 1,
        Waiting = 2,

        Owner = 1,
    };

private slots:
    void focusChanged(QTreeWidgetItem *item, int column);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

};

#endif // PARTICIPANTSPANE_H
