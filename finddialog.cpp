#include "finddialog.h"
#include "ui_finddialog.h"

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
    emit findDialogFindNext(searchString, ui->ignoreCaseCheckBox->isChecked(), ui->wrapAroundCheckBox->isChecked());
}


void FindDialog::on_findPreviousPushButton_clicked()
{
    QString searchString = ui->findTextEdit->toPlainText();
    emit findDialogFindPrev(searchString, ui->ignoreCaseCheckBox->isChecked(), ui->wrapAroundCheckBox->isChecked());
}

