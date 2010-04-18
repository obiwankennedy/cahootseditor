/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
