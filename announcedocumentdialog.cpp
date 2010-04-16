#include "announcedocumentdialog.h"
#include "ui_announcedocumentdialog.h"

#include "utilities.h"

AnnounceDocumentDialog::AnnounceDocumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnounceDocumentDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));

    QRegExp nameRx("[a-zA-Z0-9_]*");
    nameValidator = new QRegExpValidator(nameRx, 0);
    ui->nameLineEdit->setValidator(nameValidator);
    ui->prefrencesLabel->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize - 1));
}

AnnounceDocumentDialog::~AnnounceDocumentDialog()
{
    delete ui;
}

bool AnnounceDocumentDialog::isBroadcastingChecked()
{
    return ui->broadcastCheckBox->isChecked();
}

void AnnounceDocumentDialog::setAnnounceDialogInfo(QString name, bool alwaysUseThisName)
{
    ui->nameLineEdit->setText(name);
    ui->alwaysUseCheckBox->setChecked(alwaysUseThisName);
}

void AnnounceDocumentDialog::dialogAccepted()
{
    if (ui->nameLineEdit->text().length() != 0) {
        emit announceDocument(ui->nameLineEdit->text(), ui->broadcastCheckBox->checkState(), ui->alwaysUseCheckBox->checkState());
    }
}
