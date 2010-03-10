#include "connecttodocument.h"
#include "ui_connecttodocument.h"

#include <QDebug>
#include <QSettings>

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);

    ui->previousDocsComboBox->setFixedWidth(150);

    readSettings();

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp nameRx("[a-zA-Z0-9_]*");
    nameValidator = new QRegExpValidator(nameRx, 0);
    ui->usernameLineEdit->setValidator(nameValidator);

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
    QString newItem = QString("%1@%2:%3")
                      .arg(ui->usernameLineEdit->text())
                      .arg(ui->addressLineEdit->text())
                      .arg(ui->portLineEdit->text());
    if (!previousInfo.contains(newItem)) {
        if (previousInfo.size() > 4) {
            previousInfo.removeFirst();
            ui->previousDocsComboBox->removeItem(2);
        }
        ui->previousDocsComboBox->addItem(newItem);
        previousInfo.append(newItem);
    }
}

void ConnectToDocument::readSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    int length = settings.beginReadArray("infoList");
    for (int i = 0; i < length; ++i) {
        settings.setArrayIndex(i);
        previousInfo.append(settings.value("allInfo").toString());
//        ui->previousDocsComboBox->addItem(previousInfo.value(i));
        ui->previousDocsComboBox->addItem(settings.value("allInfo").toString());
    }
    settings.endArray();
}

void ConnectToDocument::writeSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    settings.beginWriteArray("infoList");
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
    list->append(ui->usernameLineEdit->text());
    list->append(ui->addressLineEdit->text());
    list->append(ui->portLineEdit->text());
    emit connectToDocumentClicked(list);
}

void ConnectToDocument::on_previousDocsComboBox_currentIndexChanged()
{
    if (ui->previousDocsComboBox->currentText() != "New...") {
        QString info = ui->previousDocsComboBox->currentText();
        QRegExp rx = QRegExp("(\\w+)@([0-9\\.]+):(\\d+)");
        if (info.contains(rx)) {
            ui->usernameLineEdit->setText(rx.cap(1).toAscii());
            ui->addressLineEdit->setText(rx.cap(2).toAscii());
            ui->portLineEdit->setText(rx.cap(3).toAscii());
        }
    }
}
