#include "participantspane.h"
#include "ui_participantspane.h"

#include "utilities.h"
#include "enu.h"

#include <QTime>
#include <QHostAddress>
#include <QTreeWidgetItem>

ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui->connectInfoLabel->hide();
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->expandAll();

    rwItem = ui->treeWidget->topLevelItem(0);
    roItem = ui->treeWidget->topLevelItem(1);
    waitItem = ui->treeWidget->topLevelItem(2);
    owner = rwItem->child(0);

    ui->treeWidget->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->connectInfoLabel->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->promotePushButton->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->demotePushButton->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));

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

void ParticipantsPane::newParticipant(QTcpSocket *socket)
{
    qDebug() << socket << " connected";
    // Add a participant, but don't make an item until we have a name.
    // Once we have a name, we updateName() (below) and insert it to the treeWidget
    Participant *participant = new Participant;
    participantList.append(participant);
    participantMap.insert(socket, participant);

    participant->socket = socket;
    participant->permissions = Enu::Waiting;
    participant->socket = socket;
    participant->address = socket->peerAddress();
}

void ParticipantsPane::addParticipant(QString name, QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);

    // This person is now ready to be added to the permissions tree view
    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, QColor::fromHsv(qrand() % 256, 190, 190));
    participant->item->setToolTip(0, QString("%1@%2").arg(name).arg(participant->address.toString()));
}

void ParticipantsPane::newParticipant(QString name)
{
    // The server has given us a name to add to the treeWidget
    Participant *participant = new Participant;
    participantList.append(participant);

    QRegExp nameRx("[a-zA-Z0-9_]*");
    if (name.contains(nameRx)) {
        participant->name = nameRx.cap(1);
    }
    else {
        return; // this is an invalid name
    }
    participant->permissions = Enu::Waiting;
    // everyone has their own colors - colors aren't consistent across participants
    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);

    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, name);
}

void ParticipantsPane::removeAllParticipants()
{
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->permissions == Enu::ReadWrite) {
            rwItem->removeChild(participantList.at(i)->item);
        }
        else if (participantList.at(i)->permissions == Enu::ReadOnly) {
            roItem->removeChild(participantList.at(i)->item);
        }
        else if (participantList.at(i)->permissions == Enu::Waiting) {
            waitItem->removeChild(participantList.at(i)->item);
        }
    }
    for (int i = 0; i < participantList.size(); i++) {
        delete participantList.at(i);
    }
    participantList.clear();
    participantMap.clear();
}

void ParticipantsPane::removeParticipant(QTcpSocket *socket)
{
    for (int i = 0; i < participantList.size(); i++) {
        if (socket == participantList.at(i)->socket) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
            }
            delete participantList.at(i);
            participantList.removeAt(i);
            participantMap.remove(socket);
            return;
        }
    }
}

void ParticipantsPane::removeParticipant(QString name)
{
    // This is a function to be used by the participants in removing participants via control messages.
    bool alreadyFound = false;
    QTcpSocket *socket;
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->name == name) {
            if (!alreadyFound) {
                alreadyFound = true;
                socket = participantList.at(i)->socket;
            }
            else {
                // we have a duplicate name, which shouldn't happen.
                // it'll be up to the server to make sure it doesn't happen.
                return;
            }
        }
    }
    removeParticipant(socket);
}

bool ParticipantsPane::canWrite(QTcpSocket *socket)
{
    return participantMap.value(socket)->permissions == Enu::ReadWrite;
}

bool ParticipantsPane::canRead(QTcpSocket *socket)
{
    return participantMap.value(socket)->permissions == Enu::ReadOnly || participantMap.value(socket)->permissions == Enu::ReadWrite;
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

    if (selectedItems.size() == 0) {
        return;
    }
    for (int i = 0; i < selectedItems.size(); i++) {
        selectedItems.at(i)->setSelected(false);
    }
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                // This should not happen, but we won't crash.
                // Instead, disable the promote button.
                ui->promotePushButton->setEnabled(false);
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                rwItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadWrite;
//                participantList.at(i)->item->setSelected(true);
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
                participantList.at(i)->permissions = Enu::ReadOnly;
                emit memberCanNowRead(participantList.at(i)->socket);
            }
            return;
        }
    }
}

void ParticipantsPane::on_demotePushButton_clicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    // find the currently selected item in the participants list

    if (selectedItems.size() == 0) {
        return;
    }
    for (int i = 0; i < selectedItems.size(); i++) {
        selectedItems.at(i)->setSelected(false);
    }
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
                participantList.at(i)->permissions = Enu::ReadOnly;
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                waitItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
                participantList.at(i)->permissions = Enu::Waiting;
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                // This should not happen, but we won't crash.
                // Instead, disable the demote button.
                ui->demotePushButton->setEnabled(false);
            }
            return;
        }
    }
}







