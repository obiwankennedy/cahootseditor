#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::changeEvent(QEvent *e)
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

void PreferencesDialog::on_changeEditor_clicked()
{

}

void PreferencesDialog::on_changeChat_clicked()
{

}

void PreferencesDialog::on_changeParticipants_clicked()
{

}

void PreferencesDialog::on_editorDefault_clicked()
{

}

void PreferencesDialog::on_chatDefault_clicked()
{

}

void PreferencesDialog::on_participantsDefault_clicked()
{

}
