/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl

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

void PreferencesDialog::setMyName(QString name)
{
    myName = name;
    ui->defaultNameLineEdit->setText(name);
}

QString PreferencesDialog::getMyName()
{
    return myName;
}

void PreferencesDialog::setAlwaysUseMyName(bool b)
{
    alwaysUseMyName = b;
    ui->useDefaultNameCheckBox->setChecked(b);
}

bool PreferencesDialog::getAlwaysUseMyName()
{
    return alwaysUseMyName;
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
//    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Set Input Font", QFontDialog::DontUseNativeDialog);
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
