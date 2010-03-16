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

    participant->permissions = Waiting;
    participant->socket = socket;

    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, "Untitled");


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
    participant->permissions = Waiting;
    // everyone has their own colors - colors aren't consistent across participants
    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);

    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, name);
}

void ParticipantsPane::updateName(QString name, QTcpSocket *socket)
{
    qDebug() << name << " joined.";
    Participant *participant = participantMap.value(socket);
    // if the item hasn't been created yet, create it with name and add it to the widget
    participant->name = name;
    // This HAS to be waitItem (enforced by having no mechanic to promote the item) until
    // we add it to the treeWidget.
    participant->item->setText(0, name);

    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);
    participant->color = participant->color.lighter(150);

    participant->item->setBackgroundColor(1, participant->color);
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
    return participantMap.value(socket)->permissions == ReadWrite;
}

bool ParticipantsPane::canRead(QTcpSocket *socket)
{
    return participantMap.value(socket)->permissions == ReadOnly || participantMap.value(socket)->permissions == ReadWrite;
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
            if (participantList.at(i)->permissions == ReadWrite) {
                // This should not happen, but we won't crash.
                // Instead, disable the promote button.
                ui->promotePushButton->setEnabled(false);
            }
            else if (participantList.at(i)->permissions == ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                rwItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = ReadWrite;
//                participantList.at(i)->item->setSelected(true);
            }
            else if (participantList.at(i)->permissions == Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
                participantList.at(i)->permissions = ReadOnly;
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
            if (participantList.at(i)->permissions == ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
                participantList.at(i)->permissions = ReadOnly;
            }
            else if (participantList.at(i)->permissions == ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                waitItem->insertChild(0, participantList.at(i)->item);
//                participantList.at(i)->item->setSelected(true);
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







