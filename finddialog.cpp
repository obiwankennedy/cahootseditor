#include "finddialog.h"
#include "ui_finddialog.h"

#include "enu.h"

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FindDialog::on_findNextPushButton_clicked()
{
    QString searchString = ui->findTextEdit->toPlainText();
    Enu::FindMode mode;
    if (ui->findModeComboBox->currentText() == "Contains") {
        mode = Enu::Contains;
    }
    else if (ui->findModeComboBox->currentText() == "Starts With") {
        mode = Enu::StartsWith;
    }
    else if (ui->findModeComboBox->currentText() == "Entire Word") {
        mode = Enu::EntireWord;
    }
    emit findDialogFindNext(searchString, ui->ignoreCaseCheckBox->isChecked(), ui->wrapAroundCheckBox->isChecked(), mode);
}


void FindDialog::on_findPreviousPushButton_clicked()
{
    Enu::FindMode mode;
    if (ui->findModeComboBox->currentText() == "Contains") {
        mode = Enu::Contains;
    }
    else if (ui->findModeComboBox->currentText() == "Starts With") {
        mode = Enu::StartsWith;
    }
    else if (ui->findModeComboBox->currentText() == "Entire Word") {
        mode = Enu::EntireWord;
    }
    QString searchString = ui->findTextEdit->toPlainText();
    emit findDialogFindPrev(searchString, ui->ignoreCaseCheckBox->isChecked(), ui->wrapAroundCheckBox->isChecked(), mode);
}

