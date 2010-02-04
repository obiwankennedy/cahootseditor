#include "connecttodocument.h"
#include "ui_connecttodocument.h"

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

void ConnectToDocument::dialogAccepted()
{
    QStringList *list = new QStringList();
    list->append(ui->usernameLineEdit->text());
    list->append(ui->addressLineEdit->text());
    list->append(ui->portLineEdit->text());
    emit connectToDocumentClicked(list);
}

