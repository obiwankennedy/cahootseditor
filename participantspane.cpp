#include "participantspane.h"
#include "ui_participantspane.h"

#include <QTime>
#include <QHostAddress>
#include <QTreeWidgetItem>

ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui->connectInfoLabel->hide();
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->expandAll();

    rwItem = ui->treeWidget->topLevelItem(0);
    roItem = ui->treeWidget->topLevelItem(1);
    waitItem = ui->treeWidget->topLevelItem(2);
    owner = rwItem->child(0);
}

ParticipantsPane::~ParticipantsPane()
{
    delete ui;
}

void ParticipantsPane::setOwnership(bool isOwner)
{
    ui->promotePushButton->setShown(isOwner);
    ui->demotePushButton->setShown(isOwner);
}

void ParticipantsPane::setConnectInfo(QString str)
{
    ui->connectInfoLabel->show();
    ui->connectInfoLabel->setText(str);
}

void ParticipantsPane::insertParticipant(QString name, QTcpSocket *socket)
{
    Participant *participant = new Participant;
    participantList.append(participant);

    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);
    participant->name = name;
    participant->item = new QTreeWidgetItem(waitItem);

    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, participant->color);

    participant->permissions = Waiting;
    participant->socket = socket;
}

void ParticipantsPane::removeAllParticipants()
{
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->permissions == ReadWrite) {
            rwItem->removeChild(participantList.at(i)->item);
        }
        else if (participantList.at(i)->permissions == ReadOnly) {
            roItem->removeChild(participantList.at(i)->item);
        }
        else if (participantList.at(i)->permissions == Waiting) {
            waitItem->removeChild(participantList.at(i)->item);
        }
    }
}

void ParticipantsPane::removeParticipant(QTcpSocket *socket)
{
    for (int i = 0; i < participantList.size(); i++) {
        if (socket == participantList.at(i)->socket) {
            if (participantList.at(i)->permissions == ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
            }
            else if (participantList.at(i)->permissions == ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
            }
            else if (participantList.at(i)->permissions == Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
            }
            delete participantList.at(i);
            participantList.removeAt(i);
            return;
        }
    }

}

void ParticipantsPane::onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *)
{    
    if (item->parent() == rwItem) {
        if (owner == item) { // if it's the owner, can't demote
            ui->demotePushButton->setDisabled(true);
        } else {
            ui->demotePushButton->setDisabled(false);
        }
        ui->promotePushButton->setDisabled(true);
    }
    else if (item->parent() == roItem) {
        ui->demotePushButton->setDisabled(false);
        ui->promotePushButton->setDisabled(false);
    }
    else if (item->parent() == waitItem) {
        ui->demotePushButton->setDisabled(true);
        ui->promotePushButton->setDisabled(false);
    }
}

void ParticipantsPane::on_promotePushButton_clicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    // find the currently selected item in the participants list
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == ReadWrite) {
                // This should not happen, but we won't crash.
                // Instead, disable the promote button.
                ui->promotePushButton->setEnabled(false);
            }
            else if (participantList.at(i)->permissions == ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                rwItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = ReadWrite;
            }
            else if (participantList.at(i)->permissions == Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = ReadOnly;
            }
            return;
        }
    }
}

void ParticipantsPane::on_demotePushButton_clicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    // find the currently selected item in the participants list
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = ReadOnly;
            }
            else if (participantList.at(i)->permissions == ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                waitItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Waiting;
            }
            else if (participantList.at(i)->permissions == Waiting) {
                // This should not happen, but we won't crash.
                // Instead, disable the demote button.
                ui->demotePushButton->setEnabled(false);
            }
            return;
        }
    }
}







