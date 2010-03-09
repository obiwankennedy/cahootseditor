#include "connecttodocument.h"
#include "ui_connecttodocument.h"

#include <QDebug>
#include <QSettings>

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);

    ui->previousDocsComboBox->setEditable(true);
    ui->previousDocsComboBox->setMaxVisibleItems(5);
    ui->previousDocsComboBox->setFixedWidth(150);

    readSettings();

    info = new ConnectInfo;

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
    writeSettings();

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
        QString newItem = QString("%1@%2:%3").arg(info->name).arg(info->address).arg(info->port);
        if (!previousInfo.contains(newItem)) {
            ui->previousDocsComboBox->addItem(newItem);
            previousInfo.append(newItem);
        }
    }
}

void ConnectToDocument::readSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    int arraySize = settings.beginReadArray("infoArray");
    for (int i = 0; i < arraySize; ++i) {
        settings.setArrayIndex(i);
        previousInfo.append(settings.value("allInfo").toString());
        ui->previousDocsComboBox->addItem(previousInfo.value(i));
    }
    settings.endArray();
}

void ConnectToDocument::writeSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    settings.beginWriteArray("infoArray");
    for (int i = 0; i < previousInfo.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("allInfo", previousInfo.value(i));
    }
    settings.endArray();
}

void ConnectToDocument::dialogAccepted()
{
    addInfo();
    QStringList *list = new QStringList();
    list->append(info->name);
    list->append(info->address);
    list->append(info->port);
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
        namePart = allInfo.section("@", 0, 0);
        QRegExp addressRx("@|:");
        addressPart = allInfo.section(addressRx, 1, 1);
        portPart = allInfo.section(":", -1);
        info->name = namePart;
        info->address = addressPart;
        info->port = portPart;
        ui->usernameLineEdit->setText(info->name);
        ui->addressLineEdit->setText(info->address);
        ui->portLineEdit->setText(info->port);
    }
}
