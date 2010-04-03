#include "announcedocumentdialog.h"
#include "ui_announcedocumentdialog.h"

AnnounceDocumentDialog::AnnounceDocumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnounceDocumentDialog)
{
    ui->setupUi(this);
}

AnnounceDocumentDialog::~AnnounceDocumentDialog()
{
    delete ui;
}
