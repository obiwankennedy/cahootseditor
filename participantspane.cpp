#include "participantspane.h"
#include "ui_participantspane.h"

ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    ui->connectInfoLabel->hide();
}

ParticipantsPane::~ParticipantsPane()
{
    delete ui;
}

void ParticipantsPane::setConnectInfo(QString str)
{
    ui->connectInfoLabel->show();
    ui->connectInfoLabel->setText(str);
}
