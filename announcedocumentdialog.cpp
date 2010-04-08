#include "announcedocumentdialog.h"
#include "ui_announcedocumentdialog.h"

AnnounceDocumentDialog::AnnounceDocumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnounceDocumentDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp nameRx("[a-zA-Z0-9_]*");
    nameValidator = new QRegExpValidator(nameRx, 0);
    ui->nameLineEdit->setValidator(nameValidator);
}

AnnounceDocumentDialog::~AnnounceDocumentDialog()
{
    delete ui;
}

void AnnounceDocumentDialog::dialogAccepted()
{
    if (ui->nameLineEdit->text().length() != 0) {
        emit announceDocument(ui->nameLineEdit->text(), ui->checkBox->checkState());
    }
}
