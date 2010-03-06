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

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(focusChanged(QTreeWidgetItem*,int)));

    ui->connectInfoLabel->hide();
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->expandAll();

//    participantList.append(new Participant());


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
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
//    QTreeWidgetItem *item = ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(name,));

//    ui->npTableWidget->setItem(ui->npTableWidget->rowCount() - 1, 1, new QTableWidgetItem(""));
//
//    ui->npTableWidget->item(ui->npTableWidget->rowCount() - 1, 1)->setBackgroundColor(QColor(qrand() % 255, qrand() % 255, qrand() % 255).lighter(150));

}

void ParticipantsPane::removeAllParticipants()
{

}

void ParticipantsPane::focusChanged(QTreeWidgetItem *item, int column)
{    
    // Is this a subcategory?
    if (column == 1) {
        return; // we want to pop up a color picker if the color row is clicked, unimplemented
    }
    bool isChildItem = item->parent().isValid();
    // Parent row (if it has a parent, -1 else)
    int parentRow = m_ui->helpTreeWidget->currentIndex().parent().row();
    // Row (if it has a parent, this is the child row)
    int row = m_ui->helpTreeWidget->currentIndex().row();

    if (isChildItem) {
        if (parentRow == ReadWrite) {
            if (row == Owner) {
                ui->demotePushButton->setEnabled(false);
                ui->promotePushButton->setEnabled(false);
            }
            else {
                ui->demotePushButton->setEnabled(true);
                ui->promotePushButton->setEnabled(false);
            }
        }
        else if (parentRow == ReadOnly) {
            ui->demotePushButton->setEnabled(true);
            ui->promotePushButton->setEnabled(true);
        }
        else if (parentRow == Waiting) {
            ui->demotePushButton->setEnabled(false);
            ui->promotePushButton->setEnabled(true);
        }
    }
}

void ParticipantsPane::on_promotePushButton_clicked()
{
    ui->treeWidget->selectedItems();
}

void ParticipantsPane::on_demotePushButton_clicked()
{

}

