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
    Qt::CaseSensitivity sensitivity;
    if (ui->ignoreCaseCheckBox->isChecked()) {
        sensitivity = Qt::CaseInsensitive;
    }
    else {
        sensitivity = Qt::CaseSensitive;
    }
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
    emit findDialogFindNext(searchString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}


void FindDialog::on_findPreviousPushButton_clicked()
{
    QString searchString = ui->findTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if (ui->ignoreCaseCheckBox->isChecked()) {
        sensitivity = Qt::CaseInsensitive;
    }
    else {
        sensitivity = Qt::CaseSensitive;
    }
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
    emit findDialogFindPrev(searchString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}

void FindDialog::on_replaceAllPushButton_clicked()
{
    QString searchString = ui->findTextEdit->toPlainText();
    QString replaceString = ui->replaceTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if (ui->ignoreCaseCheckBox->isChecked()) {
        sensitivity = Qt::CaseInsensitive;
    }
    else {
        sensitivity = Qt::CaseSensitive;
    }
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
    emit findDialogReplaceAll(searchString, replaceString, sensitivity, mode);
}

void FindDialog::on_replacePushButton_clicked()
{
    QString replaceString = ui->replaceTextEdit->toPlainText();
    emit findDialogReplace(replaceString);
}

void FindDialog::on_findReplacePushButton_clicked()
{
    QString searchString = ui->findTextEdit->toPlainText();
    QString replaceString = ui->replaceTextEdit->toPlainText();
    Qt::CaseSensitivity sensitivity;
    if (ui->ignoreCaseCheckBox->isChecked()) {
        sensitivity = Qt::CaseInsensitive;
    }
    else {
        sensitivity = Qt::CaseSensitive;
    }
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
    emit findDiaalogFindReplace(searchString, replaceString, sensitivity, ui->wrapAroundCheckBox->isChecked(), mode);
}

