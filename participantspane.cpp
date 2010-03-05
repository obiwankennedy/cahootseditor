#include "participantspane.h"
#include "ui_participantspane.h"

#include <QTime>

ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    connect(ui->rwTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(focusChanged(QTableWidgetItem *)));
    connect(ui->roTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(focusChanged(QTableWidgetItem *)));
    connect(ui->npTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(focusChanged(QTableWidgetItem *)));

    ui->connectInfoLabel->hide();

    ui->rwTableWidget->setColumnCount(2);
    ui->rwTableWidget->setRowCount(1);
    ui->rwTableWidget->setItem(0, 0, new QTableWidgetItem("Owner"));
    ui->rwTableWidget->setItem(0, 1, new QTableWidgetItem(""));
    ui->rwTableWidget->item(0, 1)->setBackgroundColor(Qt::red);
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

void ParticipantsPane::insertParticipant(QString name)
{
    ui->npTableWidget->setColumnCount(2);
    ui->npTableWidget->setRowCount(ui->npTableWidget->rowCount() + 1);
    ui->npTableWidget->setItem(ui->npTableWidget->rowCount() - 1, 0, new QTableWidgetItem(name));

    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    ui->npTableWidget->setItem(ui->npTableWidget->rowCount() - 1, 1, new QTableWidgetItem(""));

    ui->npTableWidget->item(ui->npTableWidget->rowCount() - 1, 1)->setBackgroundColor(QColor(qrand() % 255, qrand() % 255, qrand() % 255).lighter(150));
}

void ParticipantsPane::focusChanged(QTableWidgetItem *item)
{
    if (item->tableWidget() == ui->rwTableWidget) {
        ui->promotePushButton->setDisabled(true);
        ui->demotePushButton->setDisabled(false);
    }
    else if (item->tableWidget() == ui->roTableWidget) {
        ui->promotePushButton->setDisabled(false);
        ui->demotePushButton->setDisabled(false);
    }
    else if (item->tableWidget() == ui->npTableWidget) {
        ui->promotePushButton->setDisabled(false);
        ui->demotePushButton->setDisabled(true);
        // should this be false for the purpose of kicking people from the document? Maybe
    }
}

void ParticipantsPane::on_promotePushButton_clicked()
{
    // untested
    QTableWidgetItem *item;
    if (ui->roTableWidget->hasFocus()) {
        ui->rwTableWidget->setItem(ui->rwTableWidget->rowCount(), 0, item);
        ui->rwTableWidget->setItem(ui->rwTableWidget->rowCount(), 1, ui->roTableWidget->item(item->row(), 1));
        ui->roTableWidget->removeRow(ui->roTableWidget->currentItem()->row());
    }
    else if (ui->npTableWidget->hasFocus()) {
        ui->roTableWidget->setItem(ui->roTableWidget->rowCount(), 0, item);
        ui->roTableWidget->setItem(ui->roTableWidget->rowCount(), 1, ui->npTableWidget->item(item->row(), 1));
        ui->npTableWidget->removeRow(ui->npTableWidget->currentItem()->row());
    }
}

void ParticipantsPane::on_demotePushButton_clicked()
{
    // untested
    QTableWidgetItem *item;
    if (ui->rwTableWidget->hasFocus()) {
        ui->roTableWidget->setItem(ui->roTableWidget->rowCount(), 0, item);
        ui->roTableWidget->setItem(ui->roTableWidget->rowCount(), 1, ui->rwTableWidget->item(item->row(), 1));
        ui->rwTableWidget->removeRow(ui->rwTableWidget->currentItem()->row());
    }
    else if (ui->roTableWidget->hasFocus()) {
        ui->npTableWidget->setItem(ui->roTableWidget->rowCount(), 0, item);
        ui->npTableWidget->setItem(ui->roTableWidget->rowCount(), 1, ui->roTableWidget->item(item->row(), 1));
        ui->roTableWidget->removeRow(ui->roTableWidget->currentItem()->row());
    }
}

