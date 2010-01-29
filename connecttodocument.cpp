#include "connecttodocument.h"
#include "ui_connecttodocument.h"

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

ConnectToDocument::~ConnectToDocument()
{
    delete ui;
}

void ConnectToDocument::dialogAccepted()
{
    QStringList *list = new QStringList();
    list->append(ui->usernameLineEdit->text());
    list->append(ui->addressLineEdit->text());
    list->append(ui->portLineEdit->text());
    emit connectToDocumentClicked(list);
}
