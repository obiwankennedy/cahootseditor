#include "connecttodocument.h"
#include "ui_connecttodocument.h"

ConnectToDocument::ConnectToDocument(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToDocument)
{
    ui->setupUi(this);
}

ConnectToDocument::~ConnectToDocument()
{
    delete ui;
}
