#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>
#include <QTableWidgetItem>
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

    QList<Participant*> participantList;

private:
    Ui::ParticipantsPane *ui;

private slots:
    void focusChanged(QTableWidgetItem *item);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

};

#endif // PARTICIPANTSPANE_H
