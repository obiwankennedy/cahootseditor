#include "connecttodocument.h"
#include "ui_connecttodocument.h"

#include <QDebug>
#include <QSettings>

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);

    readSettings();

    info = new ConnectInfo;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp addressRx("\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    addressValidator = new QRegExpValidator(addressRx, 0);
    ui->addressLineEdit->setValidator(addressValidator);

    QRegExp portRx("\\b[0-9]{0,9}\\b");
    portValidator = new QRegExpValidator(portRx, 0);
    ui->portLineEdit->setValidator(portValidator);

    ui->previousDocsComboBox->setMaxVisibleItems(5);
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
    qDebug() << "Adding info to dialog...";
    if (ui->previousDocsComboBox->currentText() == "New...") {
        info->name = ui->usernameLineEdit->text();
        info->address = ui->addressLineEdit->text();
        info->port = ui->portLineEdit->text();
        QString newItem = QString("%1@%2:%3").arg(info->name).arg(info->address).arg(info->port);
        ui->previousDocsComboBox->addItem(newItem);
    }
    for (int i = 1; i < ui->previousDocsComboBox->count(); i++) {
        previousInfo.value(i) = ui->previousDocsComboBox->itemText(i);
    }
}

void ConnectToDocument::readSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    int size = settings.beginReadArray("infos");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        StoreInfo storeInfo;
        storeInfo.allInfo = settings.value("allInfo").toString();
        ui->previousDocsComboBox->addItem(storeInfo.allInfo);
    }
    settings.endArray();
}

void ConnectToDocument::writeSettings()
{
    QSettings settings("Cahoots", "Connect Dialog");
    for (int i = 0; i < previousInfo.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("allInfo", previousInfo.value(i));
    }
    settings.endArray();
}

// Example for the above
//void MainWindow::readSettings()
//{
//    QSettings settings("Cahoots", "MainWindow");
//    QDesktopWidget *desktop = QApplication::desktop();
//    int width = static_cast<int>(desktop->width() * 0.80);
//    int height = static_cast<int>(desktop->height() * 0.70);
//    int screenWidth = desktop->width();
//    int screenHeight = desktop->height();
//    QPoint pos = settings.value("pos", QPoint((screenWidth - width) / 2, (screenHeight - height) / 2)).toPoint();
//    QSize size = settings.value("size", QSize(width, height)).toSize();
//    resize(size);
//    move(pos);
//
//    myName = settings.value("name", "Owner").toString();
//}
//
//void MainWindow::writeSettings()
//{
//    QSettings settings("Cahoots", "MainWindow");
//    settings.setValue("pos", pos());
//    settings.setValue("size", size());
//    settings.setValue("name", myName);
//}

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

