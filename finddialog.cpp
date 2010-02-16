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


void FindDialog::on_pushButton_findNext_clicked()
{
    QString searchString = ui->textEdit_find->toPlainText();
    emit findDialogFindNext(searchString);
//    document->textEdit->find(searchString, QTextDocument::FindWholeWords);
}
