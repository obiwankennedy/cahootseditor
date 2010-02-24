#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>

namespace Ui {
    class ParticipantsPane;
}

class ParticipantsPane : public QWidget {
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setConnectInfo(QString str);

private:
    Ui::ParticipantsPane *ui;
};

#endif // PARTICIPANTSPANE_H
