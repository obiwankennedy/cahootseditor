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

bool ParticipantsPane::addParticipant(QString name, QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);

    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->socket->peerAddress() == participantList.at(i)->address && participantList.at(i)->name == name) {
            // duplicate connection, reject
            socket->disconnectFromHost();
            participantList.removeOne(participant);
            participantMap.remove(socket);
            return false;
        }
    }
    participant->name = name;
    // This person is now ready to be added to the permissions tree view
    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, name);

    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);

    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, QString("%1@%2").arg(name).arg(participant->address.toString()));

    return true;
}

QString ParticipantsPane::getNameForSocket(QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);
    return participant->name;
}

QString ParticipantsPane::getNameAddressForSocket(QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);
    return QString("%1@%2").arg(participant->name).arg(participant->address.toString());
}

void ParticipantsPane::newParticipant(QString name, QString address, QString permissions)
{
    // The server has given us a name to add to the treeWidget
    Participant *participant = new Participant;
    participantList.append(participant);

    participant->name = name;
    participant->address = QHostAddress(address);

    // everyone has their own colors - colors aren't consistent across participants
    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);

    if (permissions == "waiting") {
        participant->item = new QTreeWidgetItem(waitItem);
        participant->permissions = Enu::Waiting;
    }
    else if (permissions == "read") {
        qDebug() << "adding read-only participant " << name;
        participant->item = new QTreeWidgetItem(roItem);
        participant->permissions = Enu::ReadOnly;
    }
    else if (permissions == "write") {
        qDebug() << "adding read-write participant " << name;
        participant->item = new QTreeWidgetItem(rwItem);
        participant->permissions = Enu::ReadWrite;
    }
    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, name + "@" + address);
}

void ParticipantsPane::removeAllParticipants()
{
    for (int i = 0; i < participantList.size(); i++) {
        participantList.at(i)->item->parent()->removeChild(participantList.at(i)->item);
        delete participantList.at(i);
    }
    participantList.clear();
    participantMap.clear();
}

void ParticipantsPane::removeParticipant(QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);
    participant->item->parent()->removeChild(participant->item);
    participantMap.remove(socket);
    participantList.removeOne(participant); // this iterates through the list
    delete participant;
}

void ParticipantsPane::removeParticipant(QString name, QString address)
{
    // This is a function to be used by the participants in removing participants via control messages.
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->name == name && participantList.at(i)->address.toString() == address) {
            Participant *participant = participantList.at(i);
            participant->item->parent()->removeChild(participant->item);
            participantList.removeAt(i);
            delete participant;
        }
    }
}

void ParticipantsPane::setParticipantPermissions(QString name, QString address, QString permissions)
{
    QString fullName = name + "@" + address;
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->item->toolTip(0) == fullName) {
            Participant *participant = participantList.at(i);
            participant->item->parent()->removeChild(participant->item);

            if (permissions == "waiting") {
                waitItem->addChild(participant->item);
                participant->permissions = Enu::Waiting;
            }
            else if (permissions == "read") {
                roItem->addChild(participant->item);
                participant->permissions = Enu::ReadOnly;
            }
            else if (permissions == "write") {
                rwItem->addChild(participant->item);
                participant->permissions = Enu::ReadWrite;
            }
        }
    }
}

void ParticipantsPane::setOwnerName(QString name)
{
    qDebug() << "setting owner name: " << name;
    owner->setText(0, name);
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

    QString permissions;
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                // This should not happen, but we won't crash.
                // Instead, disable the promote button.
                ui->promotePushButton->setEnabled(false);
                permissions = "write";
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                rwItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadWrite;
                permissions = "write";
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadOnly;

                emit memberCanNowRead(participantList.at(i)->socket);
                permissions = "read";
            }
            emit memberPermissionsChanged(participantList.at(i)->socket, permissions);
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

    QString permissions;
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadOnly;
                permissions = "read";
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                waitItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::Waiting;
                permissions = "waiting";
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                // This should not happen, but we won't crash.
                // Instead, disable the demote button.
                ui->demotePushButton->setEnabled(false);
                permissions = "waiting";
            }
            emit memberPermissionsChanged(participantList.at(i)->socket, permissions);
            return;
        }
    }
}







