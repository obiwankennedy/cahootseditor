#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
    class ParticipantsPane;
}

class ParticipantsPane : public QWidget {
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setConnectInfo(QString str);

    void insertParticipant(QString name);

private:
    Ui::ParticipantsPane *ui;

private slots:
    void focusChanged(QTableWidgetItem *item);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

};

#endif // PARTICIPANTSPANE_H
