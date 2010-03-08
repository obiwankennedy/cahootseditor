#include "connecttodocument.h"
#include "ui_connecttodocument.h"

#include<QDebug>

struct connectInfo{
    QString name;
    QString address;
    QString port;
};
    connectInfo *info = new connectInfo;

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{

    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp addressRx("\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    addressValidator = new QRegExpValidator(addressRx, 0);
    ui->addressLineEdit->setValidator(addressValidator);

    QRegExp portRx("\\b[0-9]{0,9}\\b");
    portValidator = new QRegExpValidator(portRx, 0);
    ui->portLineEdit->setValidator(portValidator);
}

ConnectToDocument::~ConnectToDocument()
{
    delete ui;
}

void ConnectToDocument::setName(QString name)
{
    ui->usernameLineEdit->setText(name);
}

void ConnectToDocument::addInfo()
{
    if (ui->previousDocsComboBox->currentText() == "New...") {
        info->name = ui->usernameLineEdit->text();
        info->address = ui->addressLineEdit->text();
        info->port = ui->portLineEdit->text();
        QString newItem = QString("%1; %2; %3").arg(info->name).arg(info->address).arg(info->port);
        ui->previousDocsComboBox->addItem(newItem);
    }
    for (int i = 1; i < ui->previousDocsComboBox->count(); i++) {
        previousInfo.value(i) = ui->previousDocsComboBox->itemText(i);
    }
}

void ConnectToDocument::initializeCombobox(QStringList list)
{
    for (int i = 0; i < list.size(); i++) {
        ui->previousDocsComboBox->addItem(list.value(i));
    }
}

void ConnectToDocument::dialogAccepted()
{
    QStringList *list = new QStringList();
    list->append(info->name);
    list->append(info->address);
    list->append(info->port);
    addInfo();
    emit connectToDocumentClicked(list);
}

void ConnectToDocument::on_previousDocsComboBox_currentIndexChanged()
{
    ui->usernameLineEdit->setText("");
    ui->addressLineEdit->setText("");
    ui->portLineEdit->setText("");
    if (ui->previousDocsComboBox->currentText() != "New...") {
        QString allInfo = ui->previousDocsComboBox->currentText();
        QString namePart;
        QString addressPart;
        QString portPart;
        namePart = allInfo.section(";", 0, 0);
        addressPart = allInfo.section(";", 1, 1);
        portPart = allInfo.section(";", -1);
        info->name = namePart;
        info->address = addressPart;
        info->port = portPart;
        ui->usernameLineEdit->setText(info->name);
        ui->addressLineEdit->setText(info->address);
        ui->portLineEdit->setText(info->port);
    }
}

